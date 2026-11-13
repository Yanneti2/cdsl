#!/usr/bin/env bash

set -Eeuo pipefail

structure=(
    "bitvector"
    "bitvectorBS"
    "jacobsonrank"
    "rrr"
)

OUT="./data/"

operations=("r1" "r0" "s1" "s0")

set -e
for structure in "${structure[@]}"; do

    echo
    echo "========================================"
    echo "Running: $structure"
    echo "========================================"
    echo

    for density in d50 d30 d10 d5 d1 d.1 d99.9 d90 d95 d99; do

        for op in "${operations[@]}"; do

            Rscript preprocess.r "./data/$structure/bsb-$structure-$op-$density.csv"\
                    -o "$OUT/$structure/bsb-$structure-$op-$density.csv"\
                2> stderr

        done
    done

    echo "Finished: $structure"

done

echo
echo "All benchmarks completed successfully!"