#!/usr/bin/env bash
set -euo pipefail

PERF_DATA="${1:-perf.data}"

# 输出：time \t inst_period （每个 instructions sample 一行）
# 并在 stderr 输出 samples 数和 period 求和
perf script -i "$PERF_DATA" -F time,period,event 2>/dev/null \
| awk '
  $3 ~ /^instructions(\/|:|$)/ {
    inst = $2 + 0
    sum += inst
    n++
  }
  END {
    print "samples=" n "\ttotal_instructions_from_period=" sum > "/dev/stderr"
  }
'
