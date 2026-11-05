#!/usr/bin/env bash

set -Eeuo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(dirname "$SCRIPT_DIR")"

cd "$REPO_DIR"

scripts=(
    "bitvector/"
    "bitvectorBS/"
    "jacobsonrank/"
    "RRR/"
)

for script in "${scripts[@]}"; do
    echo
    echo "========================================"
    echo "Running: $script"
    echo "========================================"

    cd "$script"
    bash gen_bsb_benchmarks.sh

    echo "Finished: $script"

    # rm stderr
    cd ../
done

echo
echo "All benchmarks completed successfully!"