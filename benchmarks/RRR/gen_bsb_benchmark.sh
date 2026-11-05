#!/usr/bin/env bash

BIN="../../build/benchmarks/RRR/RRR-bsb"
OUT="./results"

declare -A distributions=(
    ["d50"]="5000"
    ["d30"]="3000"
    ["d10"]="1000"
    ["d5"]="500"
    ["d1"]="100"
    ["d.1"]="10"
    ["d99.9"]="9990"
    ["d90"]="9000"
    ["d95"]="9500"
    ["d99"]="9900"
)
operations=("r1" "r0")

set -e

for density in d50 d30 d10 d5 d1 d.1 d99.9 d90 d95 d99; do

    for op in "${operations[@]}"; do

        echo "Running: $op - $density"

        nice -n -20 "$BIN" "-$op" -d "${distributions[$density]}" \
            > "$OUT/rrr-bsb-$op-$density.csv" \
            2> stderr

    done
done

echo "All rrr benchmarks completed!"