
#!/usr/bin/env bash

struct1="jacobson"
struct2="bitvector"

distributions=(".1" "1" "10" "30" "5" "50" "70" "90" "95" "99.9" "99")

op=("r1" "r0" "s1" "s0")
operation_names=("rank1" "rank0" "select1" "select0")
n_operations="${#operation_names[@]}"

# Common output directory
out_dir="/tmp/comparisions"
mkdir -p "$out_dir"

# --------------------------------------------------
# Generating the operation time graphs
# --------------------------------------------------

# Uncomment this section to generate operation-time graphs.
#
flags=(--y-zero --interval-x 0,1024 --interval-y 0,32)

for i in "${!op[@]}"; do
    for d in "${distributions[@]}"; do
        f1="jacobsonrank/results/jacobson-bsb-${op[i]}-d${d}.csv"
        f2="bitvector/results/bitvector-bsb-${op[i]}-d${d}.csv"

        Rscript graph.r \
            "$f1" "$struct1" \
            "$f2" "$struct2" \
            "${flags[@]}" \
            --title "${struct1} vs ${struct2} (${operation_names[i]})" \
            -o "${out_dir}/${struct1}-${struct2}-${op[i]}-d${d}-1.png"
    done
done

echo "Operation-time graphs generated!"

# --------------------------------------------------
# Generating the memory usage graphs
# --------------------------------------------------

# Memory column for each operation in the Jacobson CSV
mem_col=("RS_Memory" "RS_Memory" "SS_Memory" "SS_Memory")

# Common graph flags
flags=(--bars normalize --y-zero --x-values 64,256,1024,4096)

for i in "${!op[@]}"; do
    for d in "${distributions[@]}"; do

        f1="jacobsonrank/results/jacobson-bsb-${op[i]}-d${d}.csv"
        f2="bitvector/results/bitvector-bsb-${op[i]}-d${d}.csv"

        Rscript graph.r \
            "$f2" "$struct2" \
            "$f1" "$struct1" \
            --y-axis "BV_Memory,${mem_col[i]}" \
            "${flags[@]}" \
            --title "${struct1} vs ${struct2} memory (${operation_names[i]}) d=${d}" \
            -o "${out_dir}/mem-${op[i]}-d${d}.png" \
            --legend "Structure"

    done
done

echo "Memory usage graphs generated!"

