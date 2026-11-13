#!/usr/bin/env bash

DIR="../x_naive/"
DENSITIES=(.1 1 5 10 30 50 70 90 95 99 99.9)
STRUCTURES=(bitvectorBS jacobson rrr)
OPERATIONS=(r1 r0 s1 s0)

args=()
for op in "${OPERATIONS[@]}"; do 
    for d in "${DENSITIES[@]}"; do
        for s in "${STRUCTURES[@]}"; do
            f="${DIR}${s}/${s}-${op}-d${d}.csv"
            if [ -f "$f" ]; then
                args+=("$f" "$s")
            else
                echo "aviso: $f não encontrado, ignorando" >&2
            fi
        done
    done

    Rscript table_graph.r "${args[@]}" \
    --order bs,jacobson,rrr --max-size 10000 \
    -o htable-${op}.png --table-out htable-${op}.xlsx "$@"
done
