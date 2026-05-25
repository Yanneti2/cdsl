# Benchmarking on jacobson's structure 

For more information on this implementation, access: [jacobson doc](https://github.com/Yanneti2/cdsl/wiki/Jacobson-5-Techniques-of-abstract-optimization).

## Operations Tested

- $rank0(i)$;
- $rank1(i)$;
- $select0(i)$;
- $select1(i)$;

## Compilation Flags

There is the possibility to directionalize your compilation of the executable in this directory with the use of the following flags:

- `_nbits` (chunk size == NBITS and NBITS*NBITS);
- `_nbits512` (chunk size == NBITS and 512*NBITS);
- `_log` (chunk size == LOG(N) and LOG(N)*LOG(N));
- `selectstructure` (builds and utilizes the select auxiliar data-strcuture)
- $`-v` || `--verbose`$ (prints out the information being tested such as the average time and order of the curr operation beign tested)
- $`-op` || `--operation`$ (need to be followed by the singular operation you want to test on that run)

It is mandatory to use a chunk size and a operation flag in each compilation. 

Note that some of the flags are exclusive, the benchmark will execute all the combination of your choice.
