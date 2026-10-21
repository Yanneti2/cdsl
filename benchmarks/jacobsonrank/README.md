# Benchmarking on jacobson's structure 

For more information on this implementation, access: [jacobson doc](https://github.com/Yanneti2/cdsl/wiki/Jacobson-5-Techniques-of-abstract-optimization).

## Operations Tested

- rank0(i)
- rank1(i)
- select0(i)
- select1(i)

## Runtime Flags

There is the possibility to directionalize your compilation of the executable in this directory with the use of the following flags:

| Flag | Alt Flag | Description |
|---|---|---|
| `_nbits` | None | Chunk size == NBITS and NBITS*NBITS |
| `_nbits512`| None |Chunk size == NBITS and 512*NBITS |
| `_log` | None | Chunk size == LOG(N) and LOG(N)*LOG(N) |
| `selectstructure` | None | Builds and utilizes the select auxiliar data-strcuture |
| `--verbose` | `-v` | Prints out the information being tested such as the average time and order of the curr operation beign tested |
| `--rank0` | `-r0` | Tests out only the rank0 operation |
| `--rank1` | `-r1` | Tests out only the rank1 operation |
| `--select0` | `-s0` | Tests out only the select0 operation |
| `--select1` | `-s1` | Tests out only the select1 operation |

It is mandatory to use a chunk size and a operation flag in each compilation. Note that some of the flags are exclusive, the benchmark will execute all the combinations of your choice.
