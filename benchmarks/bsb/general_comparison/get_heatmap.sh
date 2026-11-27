#!/usr/bin/env bash

DIR="../x_naive/"
MEM_DIR="../memory_overhead/"
OUT_DIR="graphs/map_heat"
DENSITIES=(.1 1 5 10 30 50 70 90 95 99 99.9)
STRUCTURES=(bitvectorBS jacobson rrr)
OPERATIONS=(r1 r0 s1 s0)

mkdir -p "$OUT_DIR"

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

    mem_args=()
    mem_csv="${MEM_DIR}memory-${op}.csv"
    if [ -f "$mem_csv" ]; then
        mem_args=(
            --memory "$mem_csv"
            --memory-contours "${OUT_DIR}/heatmap-${op}-contours.png"
            --memory-output "${OUT_DIR}/heatmap-${op}-mem.png"
            --memory-3d "${OUT_DIR}/heatmap-${op}-3d.png"
        )
    fi

    Rscript heatmap.r "${args[@]}" \
    --order bs,jacobson,rrr --max-size 10000 \
    -t "Choosing Rank and Select Structures (${op})" \
    -o "${OUT_DIR}/heatmap-${op}.png" \
    "${mem_args[@]}" \
    "$@" &
done
wait
