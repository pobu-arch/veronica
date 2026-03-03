#!/usr/bin/env bash
set -euo pipefail

# 用法（仅 MHz）：
#   ./core_freq.sh                # 默认 3450 (MHz)
#   ./core_freq.sh 2000           # 指定频率 (MHz)
#   F_MHZ=2000 ./core_freq.sh     # 也支持环境变量
F_MHZ="${1:-${F_MHZ:-3450}}"

if ! [[ "$F_MHZ" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
  echo "[!] Invalid MHz value: $F_MHZ"
  exit 1
fi

# MHz -> kHz（写 sysfs 用）
F_KHZ="$(awk -v mhz="$F_MHZ" 'BEGIN { printf "%.0f", mhz*1000.0 }')"
if ! [[ "$F_KHZ" =~ ^[0-9]+$ ]] || (( F_KHZ <= 0 )); then
  echo "[!] Invalid converted kHz value: $F_KHZ"
  exit 1
fi

# 自动提权
if [[ "${EUID}" -ne 0 ]]; then
  exec sudo -E bash "$0" "$@"
fi

fmt_khz_to_mhz() {
  # 输入: kHz（整数）
  awk -v khz="$1" 'BEGIN { printf "%.0f", khz/1000.0 }'
}

readf() { # readf <path> <default>
  local path="$1" def="${2:-n/a}"
  cat "$path" 2>/dev/null || echo "$def"
}

writef() { # writef <path> <value> <policy_path>
  local path="$1" val="$2" pol="${3:-}"
  if ! echo "$val" > "$path" 2>/dev/null; then
    echo "[!] write failed: $path <= $val"
    if [[ -n "$pol" && -d "$pol" ]]; then
      echo "    policy=$(basename "$pol") driver=$(readf "$pol/scaling_driver")"
      echo "    governors=$(readf "$pol/scaling_available_governors")"
      echo "    cpuinfo_min=$(readf "$pol/cpuinfo_min_freq") cpuinfo_max=$(readf "$pol/cpuinfo_max_freq")"
      if [[ -f "$pol/scaling_available_frequencies" ]]; then
        echo "    avail_freqs=$(tr '\n' ' ' < "$pol/scaling_available_frequencies" 2>/dev/null || true)"
      fi
      echo "    cur_gov=$(readf "$pol/scaling_governor") min=$(readf "$pol/scaling_min_freq") max=$(readf "$pol/scaling_max_freq")"
    fi
    return 1
  fi
}

nearest_supported_freq() { # nearest_supported_freq <policy_path> <target_khz>
  local pol="$1" target="$2"
  if [[ -f "$pol/scaling_available_frequencies" ]]; then
    # 选最接近 target 的合法频点
    awk -v t="$target" '
      BEGIN{best=""; bestd=1e18}
      {
        for(i=1;i<=NF;i++){
          f=$i; d=(f-t); if(d<0)d=-d;
          if(d<bestd){bestd=d; best=f}
        }
      }
      END{ if(best!="") print best; }
    ' "$pol/scaling_available_frequencies"
    return 0
  fi
  # 没有列表就返回原值（后续会再做 min/max clamp）
  echo "$target"
}

clamp_to_cpuinfo_range() { # clamp_to_cpuinfo_range <policy_path> <khz>
  local pol="$1" khz="$2"
  local minf maxf
  minf="$(readf "$pol/cpuinfo_min_freq" "")"
  maxf="$(readf "$pol/cpuinfo_max_freq" "")"
  if [[ "$minf" =~ ^[0-9]+$ && "$maxf" =~ ^[0-9]+$ && "$khz" =~ ^[0-9]+$ ]]; then
    if (( khz < minf )); then echo "$minf"; return 0; fi
    if (( khz > maxf )); then echo "$maxf"; return 0; fi
  fi
  echo "$khz"
}

lock_policy_freq() { # lock_policy_freq <policy_path> <req_khz>
  local p="$1" req="$2"
  local cur_min cur_max
  cur_min="$(readf "$p/scaling_min_freq" "")"
  cur_max="$(readf "$p/scaling_max_freq" "")"

  # 目标是把 min/max 都锁到 req，同时避免触发 min<=max 约束错误
  if [[ "$cur_min" =~ ^[0-9]+$ && "$cur_max" =~ ^[0-9]+$ ]]; then
    if (( req >= cur_min )); then
      writef "$p/scaling_max_freq" "$req" "$p" || return 1
      writef "$p/scaling_min_freq" "$req" "$p" || return 1
    else
      writef "$p/scaling_min_freq" "$req" "$p" || return 1
      writef "$p/scaling_max_freq" "$req" "$p" || return 1
    fi
  else
    # 读不到当前值时，双序兜底
    ( writef "$p/scaling_max_freq" "$req" "$p" && writef "$p/scaling_min_freq" "$req" "$p" ) \
      || ( writef "$p/scaling_min_freq" "$req" "$p" && writef "$p/scaling_max_freq" "$req" "$p" ) \
      || return 1
  fi
}

echo "[+] Target frequency request: ${F_MHZ} MHz (${F_KHZ} kHz, $(fmt_khz_to_mhz "$F_KHZ") MHz)"
echo

policies=(/sys/devices/system/cpu/cpufreq/policy*)
if [[ ! -d "${policies[0]}" ]]; then
  echo "[!] No cpufreq policy found under /sys/devices/system/cpu/cpufreq/ ."
  echo "    This kernel/CPU might not expose cpufreq, or driver is not loaded."
  exit 1
fi

errs=0

# 1) 设置所有 policy（尽量选择“合法频点”，并在不支持 userspace 时降级）
for p in "${policies[@]}"; do
  [[ -d "$p" ]] || continue

  req="$F_KHZ"
  req="$(clamp_to_cpuinfo_range "$p" "$req")"
  req="$(nearest_supported_freq "$p" "$req")"
  req="$(clamp_to_cpuinfo_range "$p" "$req")"

  gov_avail="$(readf "$p/scaling_available_governors" "")"
  has_userspace=0
  if [[ -n "$gov_avail" ]] && grep -qw userspace <<<"$gov_avail"; then
    has_userspace=1
  fi

  # 尽量用 userspace + setspeed（若支持）
  if [[ $has_userspace -eq 1 && -f "$p/scaling_setspeed" ]]; then
    writef "$p/scaling_governor" "userspace" "$p" || { errs=$((errs+1)); continue; }
    writef "$p/scaling_setspeed" "$req" "$p" || { errs=$((errs+1)); continue; }
  else
    # 降级：锁 min/max（通常更通用）
    if [[ $has_userspace -eq 1 ]]; then
      writef "$p/scaling_governor" "userspace" "$p" || true
    fi
    lock_policy_freq "$p" "$req" || { errs=$((errs+1)); continue; }
  fi
done

echo
if [[ $errs -ne 0 ]]; then
  echo "[!] Apply finished with $errs error(s). See messages above."
else
  echo "[+] Applied successfully."
fi
echo

# 2) 输出 policy 汇总 + 全核频率（便于确认）
echo "=== cpufreq policy summary ==="
for p in "${policies[@]}"; do
  [[ -d "$p" ]] || continue

  gov="$(readf "$p/scaling_governor")"
  drv="$(readf "$p/scaling_driver")"
  minf="$(readf "$p/scaling_min_freq")"
  maxf="$(readf "$p/scaling_max_freq")"

  curf="n/a"
  if [[ -f "$p/scaling_cur_freq" ]]; then
    curf="$(readf "$p/scaling_cur_freq")"
  elif [[ -f "$p/cpuinfo_cur_freq" ]]; then
    curf="$(readf "$p/cpuinfo_cur_freq")"
  fi

  acpus="$(readf "$p/affected_cpus" "?")"

  printf "%s  drv=%s  gov=%s  min=%s(%sMHz)  max=%s(%sMHz)  cur=%s(%sMHz)  cpus=[%s]\n" \
    "$(basename "$p")" \
    "$drv" \
    "$gov" \
    "$minf" "$( [[ "$minf" =~ ^[0-9]+$ ]] && fmt_khz_to_mhz "$minf" || echo "n/a" )" \
    "$maxf" "$( [[ "$maxf" =~ ^[0-9]+$ ]] && fmt_khz_to_mhz "$maxf" || echo "n/a" )" \
    "$curf" "$( [[ "$curf" =~ ^[0-9]+$ ]] && fmt_khz_to_mhz "$curf" || echo "n/a" )" \
    "$acpus"
done

echo
echo "=== per-CPU current freq (best-effort) ==="
for cpu in /sys/devices/system/cpu/cpu[0-9]*; do
  [[ -d "$cpu/cpufreq" ]] || continue
  id="$(basename "$cpu" | sed 's/^cpu//')"

  cur="n/a"
  if [[ -f "$cpu/cpufreq/scaling_cur_freq" ]]; then
    cur="$(readf "$cpu/cpufreq/scaling_cur_freq")"
  elif [[ -f "$cpu/cpufreq/cpuinfo_cur_freq" ]]; then
    cur="$(readf "$cpu/cpufreq/cpuinfo_cur_freq")"
  fi

  if [[ "$cur" =~ ^[0-9]+$ ]]; then
    printf "cpu%-4s  %8s kHz  (%s MHz)\n" "$id" "$cur" "$(fmt_khz_to_mhz "$cur")"
  else
    printf "cpu%-4s  %s\n" "$id" "$cur"
  fi
done

# 若有错误，保持非 0 退出码，方便自动化脚本捕获
[[ $errs -eq 0 ]]