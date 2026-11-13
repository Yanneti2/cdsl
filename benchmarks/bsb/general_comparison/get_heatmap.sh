#!/usr/bin/env bash

DIR="../x_naive/"
DENSITIES=(.1 1 5 10 30 50 70 90 95 99 99.9)
STRUCTURES=(bitvectorBS jacobson rrr)
OPERATIONS=(r1 r0 s1 s0)

declare -A NAME=( [bitvectorBS]=bs [jacobson]=jacobson [rrr]=rrr )

for op in "${OPERATIONS[@]}"; do 
    args=()
    for d in "${DENSITIES[@]}"; do
        for s in "${STRUCTURES[@]}"; do
            f="${DIR}${s}/${s}-${op}-d${d}.csv"
            if [ -f "$f" ]; then
                args+=("$f" "${NAME[$s]}")
            else
                echo "aviso: $f não encontrado, ignorando" >&2
            fi
        done
    done

    [ ${#args[@]} -eq 0 ] && { echo "nenhum arquivo para $op" >&2; continue; }

    Rscript heatmap.r "${args[@]}" \
    --order bs,jacobson,rrr --max-size 10000 \
    -t "Choosing Rank and Select Structures (${op})" \
    -o "heatmap-${op}.png" "$@"
done
