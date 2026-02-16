#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

RESULTS_DIR="${1:-${ROOT_DIR}/_results/speccpu_2017}"
OUT_FILE="${2:-${RESULTS_DIR%/}/total_instructions.log}"
INSTS_SCRIPT="${SCRIPT_DIR}/insts_count_from_perf_data.sh"

if [[ ! -x "$INSTS_SCRIPT" ]]; then
  echo "Missing or not executable: $INSTS_SCRIPT" >&2
  exit 1
fi

mkdir -p "$(dirname "$OUT_FILE")"
echo -e "# benchmark\tsamples\ttotal_instructions\tperf_path" > "$OUT_FILE"

shopt -s nullglob
processed=0

for bench_dir in "${RESULTS_DIR%/}"/*/; do
  bench="$(basename "$bench_dir")"

  perf_path="${bench_dir%/}/perf.data"
  if [[ ! -f "$perf_path" ]]; then
    perf_found="$(find "$bench_dir" -maxdepth 2 -type f -name perf.data | head -n1 || true)"
    if [[ -n "$perf_found" ]]; then
      perf_path="$perf_found"
    else
      echo -e "${bench}\t-\t-\t(none)" >> "$OUT_FILE"
      continue
    fi
  fi

  # 同时捕获 stdout + stderr，避免丢失统计信息
  out="$("$INSTS_SCRIPT" "$perf_path" 2>&1 || true)"

  # 更稳健的提取方式，直接抓取键=数字
  samples="$(printf '%s\n' "$out" | grep -oE 'samples=[0-9]+' | head -n1 | cut -d= -f2 || true)"
  total="$(printf '%s\n' "$out" | grep -oE 'total_instructions_from_period=[0-9]+' | head -n1 | cut -d= -f2 || true)"

  if [[ -n "${samples:-}" && -n "${total:-}" ]]; then
    echo -e "${bench}\t${samples}\t${total}\t${perf_path}" >> "$OUT_FILE"
    processed=$((processed+1))
  else
    # 解析失败时仍保留路径，便于人工核对
    echo -e "${bench}\t-\t-\t${perf_path}" >> "$OUT_FILE"
  fi
done

if command -v column >/dev/null 2>&1; then
  pretty="${OUT_FILE%.*}_pretty.log"
  { sed -e '1s/^# //' "$OUT_FILE" | column -t -s $'\t'; } \
  | sed -e '1s/^/# /' > "$pretty"
  echo "Also wrote aligned table to $pretty"
fi

echo "Processed ${processed} benchmarks. Wrote ${OUT_FILE}"