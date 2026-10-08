# Benchmarking on Parentheses_Tree (bp) structure



## Runtime Flags

In the following table, there are documented the functions benchmarked, its respectifull full and alternative flags for file execution and generic return type expected. By default, the benchmark tests run the functions with ParenthesesTreeRMMQ class for each and every position of the PT's `BitVector` in random order and its output is structured in a `.csv` type, with: `order`;`average_op_time` per line.

| Function | Flag | Alt Flag | Description |
|---|---|---|---|
| `None` | `--verbose` | `-v` | How the functions passed by this flags were tested |
| `*` | `--compare`| `-cmp` | Run the test with the naive and rmMq structures, comparatively |
| `*` | `--all` | `-a` | Run the tests to all the functions in the ParenthesesTree implementation |
| `ParenthesesTree`(`string` || `BitVector&`) | `--constructor` || `--builders` | `-c` || `-b` | ParenthesesTree class initilizers |
| `is_bp()` | `--valid` | `isbp` | Returns true if valid ParenthesesTree |
| `backward_search(size_t i, unsigned long long d)` | `--backwardssearch` | `bwds` | Returns a ULL index |
| `enclose(ULL i)` | `--enclose` | `-en` | Returns a ULL index |
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
| `close(ULL i)` | `--close` | `-cl` | Returns a ULL index |
| `deepestnode(size_t v)` | `--deepestnode` | `dpn` | Returns a size_t id of a node |

If no flags were to be used to execute this executable, it should appear an error:

```
terminate called after throwing an instance of 'std::invalid_argument'
  what():  No benchmark flags were provided, check this folder's README.md for more informations.
Aborted
```

## Notes

for more information on this implementation, access: [parentheses_tree doc](https://github.com/Yanneti2/cdsl/wiki/Gonzalo-8.2-Balanced_Parentheses)