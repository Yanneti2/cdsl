#!/usr/bin/env bash
MAX_JOBS=$(nproc)

struct1="jacobson"
struct2="RRR"
struct3="naive"
struct4="bitvectorBS"

distributions=("10" ".1" "1" "30" "5" "50" "70" "90" "95" "99.9" "99")

op=("r0" "r1" "s1" "s0")
operation_names=("rank0" "rank1" "select1" "select0")
n_operations="${#operation_names[@]}"

# Common input/output directories
data_dir="../data"
out_dir="graphs"
mkdir -p "$out_dir/memory" "$out_dir/time" "$out_dir/ctime"

# --------------------------------------------------
# Generating the operation time graphs
# --------------------------------------------------

flags=(--y-zero --interval-x 0,10000 --interval-y 0,300 --legend "Structure" --y-label "Time (ns)")

# CHRONO
for i in "${!op[@]}"; do
    for d in "${distributions[@]}"; do
        f1="${data_dir}/jacobsonrank/bsb-jacobsonrank-1-10k-${op[i]}-d${d}.csv"
        f2="${data_dir}/rrr/bsb-rrr-1-10k-${op[i]}-d${d}.csv"
        f3="${data_dir}/bitvector/bsb-bitvector-1-10k-${op[i]}-d${d}.csv"
        f4="${data_dir}/bitvectorBS/bsb-bitvectorBS-1-10k-${op[i]}-d${d}.csv"

        args=()
        [ -f "$f1" ] && args+=("$f1" "$struct1")
        [ -f "$f2" ] && args+=("$f2" "$struct2")
        [ -f "$f3" ] && args+=("$f3" "$struct3")
        [ -f "$f4" ] && args+=("$f4" "$struct4")

        [ ${#args[@]} -eq 0 ] && continue

        Rscript main.r \
            "${args[@]}" \
            "${flags[@]}" \
            --title "${operation_names[i]} bitvector comparisons (density = ${d}) (chrono)" \
            -o "${out_dir}/time/${struct1}-${struct2}-${struct3}-${struct4}-${op[i]}-chrono-d${d}-1.png" &

        if (( $(jobs -rp | wc -l) >= MAX_JOBS )); then
            wait -n
        fi
    done
done
wait

# CTIME (uncomment to generate ctime graphs)
# for i in "${!op[@]}"; do
#     for d in "${distributions[@]}"; do
#         f1="${data_dir}/jacobsonrank/ctime/bsb-jacobsonrank-ctime-${op[i]}-d${d}.csv"
#         f2="${data_dir}/rrr/ctime/bsb-rrr-ctime-${op[i]}-d${d}.csv"
#         f3="${data_dir}/bitvector/ctime/bsb-bitvector-ctime-${op[i]}-d${d}.csv"
#         f4="${data_dir}/bitvectorBS/ctime/bsb-bitvectorBS-ctime-${op[i]}-d${d}.csv"
#
#         args=()
#         [ -f "$f1" ] && args+=("$f1" "$struct1")
#         [ -f "$f2" ] && args+=("$f2" "$struct2")
#         [ -f "$f3" ] && args+=("$f3" "$struct3")
#         [ -f "$f4" ] && args+=("$f4" "$struct4")
#
#         [ ${#args[@]} -eq 0 ] && continue
#
#         Rscript main.r \
#             "${args[@]}" \
#             "${flags[@]}" \
#             --title "${operation_names[i]} bitvector comparisons density = ${d} (ctime)" \
#             -o "${out_dir}/ctime/${struct1}-${struct2}-${struct3}-${struct4}-${op[i]}-ctime-d${d}-1.png" &
#
#         if (( $(jobs -rp | wc -l) >= MAX_JOBS )); then
#             wait -n
#         fi
#     done
# done
# wait

echo "Operation-time graphs generated!"

# --------------------------------------------------
# Generating the memory usage graphs
# --------------------------------------------------

# Memory column for each operation in the Jacobson CSV
mem_col=("RS_Memory" "RS_Memory" "SS_Memory" "SS_Memory")

# Common graph flags
flags=(--bars normalize --baseline "$struct3" --y-zero --x-values 64,1024,8192)

for i in "${!op[@]}"; do
    for d in "${distributions[@]}"; do
        f1="${data_dir}/jacobsonrank/bsb-jacobsonrank-1-10k-${op[i]}-d${d}.csv"
        f2="${data_dir}/rrr/bsb-rrr-1-10k-${op[i]}-d${d}.csv"
        f3="${data_dir}/bitvector/bsb-bitvector-1-10k-${op[i]}-d${d}.csv"
        f4="${data_dir}/bitvectorBS/bsb-bitvectorBS-1-10k-${op[i]}-d${d}.csv"

        if [[ ! -f "$f1" || ! -f "$f3" || ! -f "$f4" ]]; then
            continue
        fi

        Rscript main.r \
            "$f1" "$struct1" \
            "$f3" "$struct3" \
            "$f4" "$struct4" \
            --y-axis "${mem_col[i]},BV_Memory,BV_Memory" \
            "${flags[@]}" \
            --title "Memory Overhead Comparison (${operation_names[i]}; d=${d})" \
            -o "${out_dir}/memory/mem-${op[i]}-d${d}.png" \
            --legend "Structure" &

        if (( $(jobs -rp | wc -l) >= MAX_JOBS )); then
            wait -n
        fi
    done
done
wait

echo "Memory usage graphs generated!"
