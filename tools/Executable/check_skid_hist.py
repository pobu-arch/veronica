#!/usr/bin/env python3
import argparse
import os
import re
import sys
from collections import Counter

HEX_RE = re.compile(r"^(?:0x)?[0-9a-fA-F]+$")


def parse_hex_token(tok: str):
    tok = tok.strip()
    if not tok:
        return None
    if tok.endswith(":"):
        tok = tok[:-1]
    if not HEX_RE.match(tok):
        return None
    try:
        return int(tok, 16)
    except ValueError:
        return None


def extract_ip_auto(line: str):
    """
    Best-effort IP extractor for perf script output.
    Works for:
      - lines that are just: <hex_ip>
      - lines like: comm pid/tid time: ip sym (dso)
    """
    parts = line.strip().split()
    if not parts:
        return None

    # Fast-path: single token that is hex => ip-only format.
    if len(parts) == 1:
        return parse_hex_token(parts[0])

    # Heuristics: find the first "pure hex token" after the time field.
    for i, tok in enumerate(parts):
        if "/" in tok:
            continue
        if "." in tok and tok.endswith(":"):
            if i + 1 < len(parts):
                ip = parse_hex_token(parts[i + 1])
                if ip is not None:
                    return ip
        ip = parse_hex_token(tok)
        if ip is not None and ip >= 0x1000:
            return ip
    return None


def _bucket_nonneg(v: float):
    """Map non-negative value to [0,2), [2,4), [4,8), ..."""
    if v < 2.0:
        return (0.0, 2.0)
    lo = 2.0
    hi = 4.0
    while v >= hi:
        lo = hi
        hi *= 2.0
    return (lo, hi)


def print_aggregated(hist: Counter, total: int, unit_name: str, scale: float):
    """
    Aggregate histogram by power-of-two ranges in 'unit':
      positive: [0,2), [2,4), [4,8), ...
      negative: [-2,0), [-4,-2), [-8,-4), ...
    scale:
      delta_unit = delta_bytes / scale
      (instructions: scale=bytes_per_inst; bytes: scale=1)
    """
    pos = Counter()
    neg = Counter()

    for delta_bytes, cnt in hist.items():
        v = float(delta_bytes) / scale
        if v >= 0:
            b = _bucket_nonneg(v)
            pos[b] += cnt
        else:
            b = _bucket_nonneg(-v)
            neg[b] += cnt

    print(f"\naggregated_distribution(unit={unit_name})")

    print("positive_ranges:")
    for (lo, hi) in sorted(pos.keys(), key=lambda x: x[0]):
        c = pos[(lo, hi)]
        pct = c * 100.0 / total
        print(f"[{lo:g},{hi:g})  count={c:9d}  pct={pct:7.3f}")

    print("negative_ranges:")
    # neg bucket (lo,hi) means real range [-hi,-lo)
    for (lo, hi) in sorted(neg.keys(), key=lambda x: x[0]):
        c = neg[(lo, hi)]
        pct = c * 100.0 / total
        print(f"[-{hi:g},-{lo:g})  count={c:9d}  pct={pct:7.3f}")


def main():
    ap = argparse.ArgumentParser(
        description="Compute skid histogram: delta = sample_ip - skid_target."
    )
    ap.add_argument(
        "--target",
        default=None,
        help="Target address (hex), e.g. 0xaaaac1501300. If omitted, use env SKID_TARGET.",
    )
    ap.add_argument(
        "--ip-field",
        type=int,
        default=0,
        help="Force IP to be taken from column N (1-based). 0 = auto-detect (default).",
    )
    ap.add_argument(
        "--top",
        type=int,
        default=50,
        help="Print top-N most common raw deltas (default: 50). Use 0 to disable raw list.",
    )
    ap.add_argument(
        "--bytes-per-inst",
        type=int,
        default=0,
        help="If non-zero, print instruction-scaled values (use 4 for AArch64/RV64).",
    )
    args = ap.parse_args()

    target_s = args.target or os.getenv("SKID_TARGET")
    if not target_s:
        print("error: missing target. Provide --target or set env SKID_TARGET.", file=sys.stderr)
        return 2
    try:
        target = int(target_s, 16)
    except ValueError:
        print(f"error: invalid target hex: {target_s}", file=sys.stderr)
        return 2

    hist = Counter()
    total = 0
    bad = 0

    for line in sys.stdin:
        if not line.strip():
            continue

        ip = None
        if args.ip_field > 0:
            parts = line.strip().split()
            idx = args.ip_field - 1
            if idx < len(parts):
                ip = parse_hex_token(parts[idx])
        else:
            ip = extract_ip_auto(line)

        if ip is None:
            bad += 1
            continue

        hist[ip - target] += 1
        total += 1

    if total == 0:
        print("error: no IP samples parsed. Consider using --ip-field or check perf script format.", file=sys.stderr)
        print(f"note: unparsed_lines={bad}", file=sys.stderr)
        return 1

    print(f"target={hex(target)} total_samples={total} unparsed_lines={bad}")
    if args.bytes_per_inst:
        print(f"bytes_per_inst={args.bytes_per_inst}")

    # New: aggregated distribution
    if args.bytes_per_inst:
        print_aggregated(hist, total, unit_name="inst", scale=float(args.bytes_per_inst))
    else:
        print_aggregated(hist, total, unit_name="bytes", scale=1.0)

    # Raw (old behavior), optional
    if args.top > 0:
        print("\nraw_top_deltas:")
        for delta, cnt in hist.most_common(args.top):
            pct = cnt * 100.0 / total
            if args.bytes_per_inst:
                inst = delta / float(args.bytes_per_inst)
                print(f"delta_bytes={delta:8d}  delta_inst={inst:9.2f}  count={cnt:9d}  pct={pct:7.3f}")
            else:
                print(f"delta_bytes={delta:8d}  count={cnt:9d}  pct={pct:7.3f}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
