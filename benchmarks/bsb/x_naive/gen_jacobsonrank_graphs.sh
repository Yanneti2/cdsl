#!/usr/bin/env bash

struct1="jacobson"
struct2="bitvector"

distributions=("10" ".1" "1"  "30" "5" "50" "70" "90" "95" "99.9" "99")

op=("r0" "r1" "s1" "s0")
operation_names=("rank1" "rank0" "select1" "select0")

for i in "${!op[@]}"; do
   for d in "${distributions[@]}"; do
       f1="../data/jacobsonrank/bsb-jacobson-${op[i]}-d${d}.csv"
       f2="../data/bitvector/bsb-bitvector-${op[i]}-d${d}.csv"

       Rscript ../graph.r \
           "$f1" "$struct1" \
           "$f2" "$struct2" \
           "${flags[@]}" \
           --title "${struct1} vs ${struct2} (${operation_names[i]}) d${d}" \
           --output "${struct1}-${struct2}-${op[i]}-d${d}.png"
   done
done

echo "Jacobson-BitVector graphs generated!"
