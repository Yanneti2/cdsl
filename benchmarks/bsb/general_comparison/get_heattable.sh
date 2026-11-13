#!/usr/bin/env bash

DENSITIES=(5 10 30 50 70 90 95 99 99.9) # .1 1

args=()
add() {  # add <arquivo> <nome>
    if [ -f "$1" ]; then
        args+=("$1" "$2")
    else
        echo "aviso: $1 não encontrado, ignorando" >&2
    fi
}

for d in "${DENSITIES[@]}"; do
    add "../../bitvector/results/bitvector-bsb-r1-d${d}.csv"     naive
    add "../../bitvectorBS/results/bitvectorBS-bsb-r1-d${d}.csv" bs
    add "../../jacobsonrank/results/jacobson-bsb-r1-d${d}.csv"   jacobson
    add "../../RRR/results/bsb-rrr-r1-d${d}.csv"                 rrr
done

Rscript heat_graph.r "${args[@]}" \
    --baseline naive --order bs,jacobson,rrr --max-size 10000 \
    -o breakeven.png --table-out breakeven_table.xlsx "$@"