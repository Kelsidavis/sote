#!/usr/bin/env bash
set -euo pipefail
CAP_LINES="${CAP_LINES:-400}"
CAP_BYTES="${CAP_BYTES:-262144}"
OUTDIR="${1:?usage: log_cap.sh OUTDIR -- cmd...}"
shift
mkdir -p "$OUTDIR"
# Run command, capture separately
# No tee into chat; we only write files then tail/cap.
( "$@" >"$OUTDIR/stdout.log" 2>"$OUTDIR/stderr.log" ) || true
# Produce safe snippets
tail -n "$CAP_LINES"  "$OUTDIR/stderr.log" > "$OUTDIR/stderr.tail.log" || true
tail -c "$CAP_BYTES"  "$OUTDIR/stderr.log" > "$OUTDIR/stderr.cap.log"  || true
tail -n "$CAP_LINES"  "$OUTDIR/stdout.log" > "$OUTDIR/stdout.tail.log" || true
tail -c "$CAP_BYTES"  "$OUTDIR/stdout.log" > "$OUTDIR/stdout.cap.log"  || true
# Print tiny summary line for the agent
echo "[log_cap] stderr: $(wc -c < "$OUTDIR/stderr.log") bytes (tail=${CAP_LINES}l, cap=${CAP_BYTES}b)"
echo "[log_cap] stdout: $(wc -c < "$OUTDIR/stdout.log") bytes (tail=${CAP_LINES}l, cap=${CAP_BYTES}b)"