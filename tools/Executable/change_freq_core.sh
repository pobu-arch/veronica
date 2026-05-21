#!/usr/bin/env bash
set -euo pipefail

# 用法：
#   ./change_freq_core.sh                 # 默认 2700 (MHz)
#   ./change_freq_core.sh 3100            # 只接受纯数字 MHz（3100 => 3100000 kHz）
#   ./change_freq_core.sh 3100.5          # 支持小数 MHz
#   DISABLE_BOOST=1 ./change_freq_core.sh  # 可选：尝试关闭 boost
RAW_F="${1:-2700}"
DISABLE_BOOST="${DISABLE_BOOST:-0}"

# 自动提权
if [[ "${EUID}" -ne 0 ]]; then
  exec sudo -E bash "$0" "$@"
fi

fmt_khz_to_mhz() {
  # 输入: kHz（整数）
  awk -v khz="$1" 'BEGIN { printf "%.0f", khz/1000.0 }'
}

normalize_target_khz() { # normalize_target_khz <input>
  local raw="$1"
  local in="$raw"

  # 仅允许纯数字 MHz（可含小数），禁止任何单位后缀。
  if [[ "$in" =~ ^[0-9]+([.][0-9]+)?$ ]]; then
    awk -v mhz="$in" 'BEGIN { printf "%.0f\n", mhz*1000 }'
    return 0
  fi

  return 1
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

target_supported_by_policy() { # target_supported_by_policy <policy_path> <khz>
  local pol="$1" khz="$2"
  local minf maxf
  minf="$(readf "$pol/cpuinfo_min_freq" "")"
  maxf="$(readf "$pol/cpuinfo_max_freq" "")"

  if [[ "$khz" =~ ^[0-9]+$ && "$minf" =~ ^[0-9]+$ && (( khz < minf )) ]]; then
    echo "[!] $(basename "$pol"): target=${khz} lower than cpuinfo_min=${minf}"
    return 1
  fi
  if [[ "$khz" =~ ^[0-9]+$ && "$maxf" =~ ^[0-9]+$ && (( khz > maxf )) ]]; then
    echo "[!] $(basename "$pol"): target=${khz} higher than cpuinfo_max=${maxf}"
    return 1
  fi

  if [[ -f "$pol/scaling_available_frequencies" ]]; then
    local avail
    avail="$(tr '\n' ' ' < "$pol/scaling_available_frequencies" 2>/dev/null || true)"
    if [[ -n "$avail" ]] && ! grep -qw "$khz" <<<"$avail"; then
      echo "[!] $(basename "$pol"): target=${khz} not in available frequencies: $avail"
      return 1
    fi
  fi

  return 0
}

set_locked_range() { # set_locked_range <min_path> <max_path> <target_khz> <policy_path>
  local min_path="$1" max_path="$2" target="$3" pol="${4:-}"
  local cur_max
  cur_max="$(readf "$max_path" "")"

  # 提频时先写 max，再写 min；降频时反过来，避免 min>max / max<min 约束导致写失败。
  if [[ "$cur_max" =~ ^[0-9]+$ && "$target" =~ ^[0-9]+$ && (( target > cur_max )) ]]; then
    writef "$max_path" "$target" "$pol" || return 1
    writef "$min_path" "$target" "$pol" || return 1
  else
    writef "$min_path" "$target" "$pol" || return 1
    writef "$max_path" "$target" "$pol" || return 1
  fi
}

maybe_disable_boost() {
  [[ "$DISABLE_BOOST" == "1" ]] || return 0

  for bpath in \
    /sys/devices/system/cpu/cpufreq/boost \
    /sys/devices/system/cpu/amd_pstate/cpb_boost; do
    [[ -w "$bpath" ]] || continue
    writef "$bpath" "0" "" || true
    echo "[+] Boost disabled via: $bpath"
  done
}

show_boost_hint() {
  [[ "$DISABLE_BOOST" == "1" ]] && return 0

  for bpath in \
    /sys/devices/system/cpu/cpufreq/boost \
    /sys/devices/system/cpu/amd_pstate/cpb_boost; do
    [[ -r "$bpath" ]] || continue
    if [[ "$(readf "$bpath" "")" == "1" ]]; then
      echo "[i] Boost is enabled ($bpath=1). Current freq may exceed requested value under load."
      echo "    For stricter capping, run with: DISABLE_BOOST=1 ./change_freq_core.sh <freq>"
      break
    fi
  done
}

if ! F="$(normalize_target_khz "$RAW_F")"; then
  echo "[!] Invalid frequency input: $RAW_F"
  echo "    Only plain numeric MHz is allowed, e.g.: 3100, 3100.5"
  exit 1
fi

if ! [[ "$F" =~ ^[0-9]+$ ]] || (( F <= 0 )); then
  echo "[!] Parsed frequency must be a positive integer in kHz, got: $F"
  exit 1
fi

echo "[i] Interpreting numeric input '$RAW_F' as MHz => ${F} kHz"

echo "[+] Target frequency request: ${F} kHz ($(fmt_khz_to_ghz "$F") GHz)"
echo

policies=(/sys/devices/system/cpu/cpufreq/policy*)
if [[ ! -d "${policies[0]}" ]]; then
  echo "[!] No cpufreq policy found under /sys/devices/system/cpu/cpufreq/ ."
  echo "    This kernel/CPU might not expose cpufreq, or driver is not loaded."
  exit 1
fi

errs=0
maybe_disable_boost
show_boost_hint

# 1) 设置所有 policy（严格按请求频率；不支持则报错）
for p in "${policies[@]}"; do
  [[ -d "$p" ]] || continue

  req="$F"
  if ! target_supported_by_policy "$p" "$req"; then
    errs=$((errs+1))
    continue
  fi

  driver="$(readf "$p/scaling_driver" "")"
  is_amd_pstate=0
  if [[ "$driver" == amd-pstate* ]]; then
    is_amd_pstate=1
  fi

  gov_avail="$(readf "$p/scaling_available_governors" "")"
  has_userspace=0
  has_perf=0
  if [[ -n "$gov_avail" ]] && grep -qw userspace <<<"$gov_avail"; then
    has_userspace=1
  fi
  if [[ -n "$gov_avail" ]] && grep -qw performance <<<"$gov_avail"; then
    has_perf=1
  fi

  # 尽量用 userspace + setspeed（若支持）
  if [[ $has_userspace -eq 1 && -f "$p/scaling_setspeed" ]]; then
    writef "$p/scaling_governor" "userspace" "$p" || { errs=$((errs+1)); continue; }
    writef "$p/scaling_setspeed" "$req" "$p" || { errs=$((errs+1)); continue; }

    applied_setspeed="$(readf "$p/scaling_setspeed" "")"
    if [[ "$applied_setspeed" =~ ^[0-9]+$ && "$applied_setspeed" != "$req" ]]; then
      echo "[!] $(basename "$p"): setspeed clamped to ${applied_setspeed}, requested ${req}"
      errs=$((errs+1))
      continue
    fi
  else
    # 降级：锁 min/max（通常更通用）。
    # AMD pstate/epp 通常不支持 userspace+setspeed，优先切 performance。
    if [[ $is_amd_pstate -eq 1 && $has_perf -eq 1 ]]; then
      writef "$p/scaling_governor" "performance" "$p" || true
    elif [[ $has_userspace -eq 1 ]]; then
      writef "$p/scaling_governor" "userspace" "$p" || true
    fi

    set_locked_range "$p/scaling_min_freq" "$p/scaling_max_freq" "$req" "$p" || { errs=$((errs+1)); continue; }

    applied_min="$(readf "$p/scaling_min_freq" "")"
    applied_max="$(readf "$p/scaling_max_freq" "")"
    if [[ "$applied_min" =~ ^[0-9]+$ && "$applied_max" =~ ^[0-9]+$ ]]; then
      if (( applied_min != req || applied_max != req )); then
        echo "[!] $(basename "$p"): min/max clamped to min=${applied_min} max=${applied_max}, requested ${req}"
        errs=$((errs+1))
        continue
      fi
    fi

    # AMD pstate 的专有 min/max（若可写）也同步锁定，避免接口不一致。
    if [[ $is_amd_pstate -eq 1 && -w "$p/amd_pstate_min_freq" && -w "$p/amd_pstate_max_freq" ]]; then
      set_locked_range "$p/amd_pstate_min_freq" "$p/amd_pstate_max_freq" "$req" "$p" || { errs=$((errs+1)); continue; }
    fi

    # AMD pstate-epp 常受 EPP 影响，尽量把偏好设为 performance。
    if [[ $is_amd_pstate -eq 1 && -w "$p/energy_performance_preference" ]]; then
      writef "$p/energy_performance_preference" "performance" "$p" || true
    fi
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

  extra=""
  if [[ "$drv" == amd-pstate* ]]; then
    epp="$(readf "$p/energy_performance_preference" "n/a")"
    apmin="$(readf "$p/amd_pstate_min_freq" "n/a")"
    apmax="$(readf "$p/amd_pstate_max_freq" "n/a")"
    extra="  epp=${epp}  amd_min=${apmin}  amd_max=${apmax}"
  fi

  printf "%s  drv=%s  gov=%s  min=%s(%sGHz)  max=%s(%sGHz)  cur=%s(%sGHz)  cpus=[%s]%s\n" \
    "$(basename "$p")" \
    "$drv" \
    "$gov" \
    "$minf" "$( [[ "$minf" =~ ^[0-9]+$ ]] && fmt_khz_to_ghz "$minf" || echo "n/a" )" \
    "$maxf" "$( [[ "$maxf" =~ ^[0-9]+$ ]] && fmt_khz_to_ghz "$maxf" || echo "n/a" )" \
    "$curf" "$( [[ "$curf" =~ ^[0-9]+$ ]] && fmt_khz_to_ghz "$curf" || echo "n/a" )" \
    "$acpus" \
    "$extra"
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