densities=(1 10 50 100 300 500 700 900 950 9990 990)
density_names=(.1 1 5 10 30 50 70 90 95 99.9 99)
operations=(r0 r1)

for i in "${!densities[@]}"; do
    d="${densities[$i]}"
    name="${density_names[$i]}"

    for op in "${operations[@]}"; do
        Rscript graph.r \
            "../../RRR/results/rrr-bsb-${op}-d${name}.csv" \
            "../../bitvector/results/bitvector-bsb-${op}-d${name}.csv" \
            "rrr-${op}-d${name}.csv" \
            "rrr-${op}-d${name}.png"
    done
done