#!/usr/bin/env bash

struct1="jacobson"
struct2="bitvector"
struct3="bitvectorBS"
struct4="RRR"

distributions=("10" ".1" "1"  "30" "5" "50" "70" "90" "95" "99.9" "99")
op=("r0" "r1" "s1" "s0")
operation_names=("rank1" "rank0" "select1" "select0")
n_operations="${#operation_names[@]}"

# Common output directory
# out_dir="/tmp/comparisions"

out_mem="graphs/memory"
out_time="graphs/time"

mkdir -p "$out_dir"

# --------------------------------------------------
# Generating the operation time graphs
# --------------------------------------------------

# Uncomment this section to generate operation-time graphs.

# flags=(--y-zero --interval-x 0,10000 --intersect-all --min-run 20) # --interval-x 0,2048 --interval-y 0,140

# for i in "${!op[@]}"; do
#    for d in "${distributions[@]}"; do
#        f1="../data/jacobsonrank/bsb-jacobsonrank-${op[i]}-d${d}.csv"
#        f2="../data/bitvector/bsb-bitvector-${op[i]}-d${d}.csv"
#        f3="../data/bitvectorBS/bsb-bitvectorBS-${op[i]}-d${d}.csv"
#        f4="../data/rrr/bsb-rrr-${op[i]}-d${d}.csv"

#        Rscript main.r \
#            "$f1" "$struct1" \
#            "$f2" "$struct2" \
#            "$f3" "$struct3" \
#            "$f4" "$struct4" \
#            "${flags[@]}" \
#            --title "${struct1} vs ${struct2} vs ${struct3} vs ${struct4} (${operation_names[i]}) d${d}" \
#            -o "${out_time}/${struct1}-${struct2}-${struct3}-${struct4}-${op[i]}-d${d}-1.png"
#    done
# done

# echo "Operation-time graphs generated!"

# --------------------------------------------------
# Generating the memory usage graphs
# --------------------------------------------------

# RUNNING RANK BELOW

# Memory column for each operation in the Jacobson CSV
# mem_col=("RS_Memory" "RS_Memory" "SS_Memory" "SS_Memory")

# # Common graph flags
# flags=(--bars normalize --y-zero --x-values 64,1024,8192,16368,262128)

# for i in "${!op[@]}"; do
#     for d in "${distributions[@]}"; do

#         f2="../data/bitvector/bsb-bitvector-${op[i]}-d${d}.csv"
#         f1="../data/jacobsonrank/bsb-jacobsonrank-${op[i]}-d${d}.csv"
#         f3="../data/bitvectorBS/bsb-bitvectorBS-${op[i]}-d${d}.csv"
#         f4="../data/rrr/bsb-rrr-${op[i]}-d${d}.csv"

#         Rscript main.r \
#             "$f2" "$struct2" \
#             "$f1" "$struct1" \
#             "$f3" "$struct3" \
#             "$f4" "$struct4" \
#             --y-axis "BV_Memory,${mem_col[i]},BV_Memory,${mem_col[i]}" \
#             "${flags[@]}" \
#             --title "${struct1} vs ${struct2} vs ${struct3} vs ${struct4}\
#             memory (${operation_names[i]}) d=${d}" \
#             -o "${out_mem}/mem-${op[i]}-d${d}.png" \
#             --legend "Structure" &

#     done
# done

# RUNNING SELECT BELOW

# for i in "${!op[@]}"; do
#     for d in "${distributions[@]}"; do

#         f1="../data/jacobsonrank/bsb-jacobsonrank-${op[i]}-d${d}.csv"
#         f2="../data/bitvector/bsb-bitvector-${op[i]}-d${d}.csv"
#         f3="../data/bitvectorBS/bsb-bitvectorBS-${op[i]}-d${d}.csv"

#         Rscript main.r \
#             "$f2" "$struct2" \
#             "$f1" "$struct1" \
#             "$f3" "$struct3" \
#             --y-axis "BV_Memory,SS_Memory,BV_Memory" \
#             "${flags[@]}" \
#             --title "${struct1} vs ${struct2} vs ${struct3}\
#             memory (${operation_names[i]}) d=${d}" \
#             -o "${out_mem}/mem-${op[i]}-d${d}.png" \
#             --legend "Structure" &

#     done
# done

# echo "Memory usage graphs generated!"