# Benchmarks for cdsl

inside ~/benchmarks, we have a graph.py file, that contains the `.csv` to `.png` graph logic and a `interquartil-stats.cpp` file that holds onto our tests quality validation.

Each directory inside this directory, with its name being the respective data-structure benchmarked, is divided in:

- Directory `results` containing:
    - `.csv` files with the tests data output obtained from the executable;
    - `graphs` subdirectory with `.png` files with the results data of a given operation in a graphical visual form.
- Executable `.cpp` file with the benchmark tests logic;
- `README.md` file with the operations tested and the runtime flags.

After running `make` on the build directory, the executable should be avalable for testing purposes. For more informations on how to run these tests, have a look onto each benchmarked structure's `README.md`.

## How to use graph.py

**Requirements:** Python 3.13.5+

```bash
cd benchmarks
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Running the file supports a few arguments and their order matters. Usually, running the program to create a graph with `n` valid `.csv` files for different data follow the lines of:

```
python3 graph.py n*(.csv_file_path data_label) graph.png_file_name --log-x --log-y
```

Example:
```bash
python3 graph.py jacobsonrank/results/ns_select0_log.csv select0logyay jacobsonrank/results/rank0_log.csv rankyay jacobsonrank/results/rank1_log.csv rank1yay jacobsonrank/results/select1_log.csv select1yay coolname --log-x --log-y
```

| Flag | Mandatory | Description |
|---|---|---|
| `graph.png_file_name` | No | If not provided the .png graph name will be the last label_name from a `.csv` file provided |
| `--log-x` | No | Logarithmic x-axis | 
| `--log-y` | No | Logarithmic y-axis |

If you run a command that does not follow the standards covered by this file, yout should get an Error.

## Notes

For more informations, access [main README](../README.md).