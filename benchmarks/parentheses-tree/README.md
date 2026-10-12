# Benchmarking on Parentheses_Tree (bp) structure

For more information on this implementation, access: [parentheses_tree doc](https://github.com/Yanneti2/cdsl/wiki/Gonzalo-8.2-Balanced_Parentheses).

## Operations Tested

| Operation | Flag | Alt Flag | Return type |
|---|---|---|---|
| `enclose(ULL i)` | `--enclose` | `-en` | Returns a ULL index |
| `close(ULL i)` | `--close` | `-cl` | Returns a ULL index |
<!-- | `ParenthesesTree`(`string` || `BitVector&`) | `--constructor` || `--builders` | `-c` || `-b` | ParenthesesTree class initilizers |
| `is_bp()` | `--valid` | `isbp` | Returns true if valid ParenthesesTree |
| `backward_search(size_t i, unsigned long long d)` | `--backwardssearch` | `bwds` | Returns a ULL index |
| `parent(size_t v)` | `--parent` | `-p` | Returns a size_t id of a node |
| `isleaf(size_t v)` | `--is_leaf` | `-isl` | Returns True if node a leaf
| `subtree(size_t v)` | `--subtree` | `-sbt` | Returns a size_t id of a node |
| `leafrank(size_t v)` | `--leafrank` | `-lr` | Returns a ULL index |
| `leafnum(size_t v)` | `--leafnum` | `-ln` | Returns a ULL index |
| `leafselect(ULL i)` | `--leafselect` | `-ls` | Returns a size_t id of a node |
| `children(size_t v)` | `--children` | `-ch` | Returns a ULL index |
| `childrank(size_t v)` | `--childrank` | `-cr` | Returns a ULL index |
| `lchild(size_t v)` | `--lchild` | `-lc` | Returns a size_t id of a node |
| `isancestor(size_t u, size_t v)` | `--isancestor` | `ia` | Returns true if ancestor |
| `deepestnode(size_t v)` | `--deepestnode` | `dpn` | Returns a size_t id of a node | -->

## Runtime Flags

By default, the benchmark tests run the functions with ParenthesesTreeRMMQ class for each and every position of the PTRMMQ's `BitVector` in random order and its output is structured in a `.csv` type, with: `order`;`average_op_time` per line.

| Flag | Alt Flag | Description |
|---|---|---|
| `--verbose` | `-v` | How the functions passed by this flags were tested plus verbose output |
| `--compare`| `-cmp` | Run the test with the naive and rmMq structures, making comparison easier |
| `--all` | `-a` | Run the tests to all the functions for the ParenthesesTreeRMMQ implementation |
| `--naive` | `-n` | Run the test to all function for the naive ParenthesesTree implementation |

If no flags were to be used to execute this executable, it should appear the error:

```
terminate called after throwing an instance of 'std::invalid_argument'
  what():  No benchmark flags were provided, check this folder's README.md for more informations.
Aborted
```
