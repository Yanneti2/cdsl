# Benchmarking on wavelet-tree

For more information on this implementation, access: [wavelet-tree documentation](https://github.com/Yanneti2/cdsl/wiki/Gonzalo-6.2-Wavelet-Trees).

## Operations Tested

| Operation | Flag | Alt Flag | Return type |
|---|---|---|---|
| `rank(uint64_t c, size_t i)` | `--rankc` + `--succinct` | `-rc` + `-s` | The number of occurences of the char c until the i'th -1 position (exclusive) | 
| `select(uint64_t c,  size_t i)` | `--selectc` + `--succinct` | `-sc` + `s` | The size_t position of the i'th char c in the original string |
| `access(size_t i)` | `--access` + `--succinct` | `-acs` + `s` | The char c preseneted in the i'th position of the original string |

## Runtime Flags

By default, the benchmarks tests run the funcions with WaveletTree class for each and every position of the PTRMMQ's `BitVector` in random order and its output is structures in a `.csv` type with: `order`;`average_op_time` per line.

| Flag | Alt Flag | Description |
|---|---|---|
| `--verbose` | `-v` | How the functions passed by this flags were tested plus verbose output |
| `--succinct` | `-s` | Run the tests with only the WaveletTreeSuccint class |
| `--both` | `-b` | Run the test with the WaveletTree and WaveletTreeSuccinct structures, making comparison easier |
| `--all` | `-a` | Run the test to all functions for the, by default, WaveletTree class (can be modded by -b && -s) |
| `--interquartile` | `-i` | Run the tests with the interquartile method |
| `--mean` | `-m` | Run the tests with a simple average time result |

If no flags were to be used to execute this executable, it should appear the error:

```
terminate called after throwing an instance of 'std::invalid_argument'
  what():  No benchmark flags were provided, check this folder's README.md for more informations.
Aborted
```

If no time running time measurement is to be determined, it should appear the error:

```
 terminate called after throwing an instance of 'std::invalid_argument'
  what():  No time calculation method provided, check this folder's README.md for more informations.
Aborted 
```