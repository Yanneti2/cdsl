# Benchmarks for cdsl

inside ~/benchmarks, we have a `graph.py` file, that contains the `.csv` to `.png` graph creation logic and a `interquartil-stats.cpp` file that holds onto our tests quality validation.

Each directory here, with its name being the respective compressed-data-structure benchmarked, is divided in:

- Directory `results` containing:
    - `.csv` files with the tests data output obtained from the executable;
    - `graphs` subdirectory with `.png` files with the results data of a given operation in a graphical visual form.
- Executable `.cpp` file with the benchmark tests logic;
- `README.md` file with the operations tested and the runtime flags.

After running `make` on the build directory, the executable should be avalable for testing purposes. For more informations on how to run these tests, have a look onto each benchmarked structure's `README.md`.

## Using graph.py

**Requirements:** Python 3.13.5+

```bash
cd benchmarks
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Running the file supports a few arguments and their order matters. 

| Flag | Mandatory | Description |
|---|---|---|
| `graph.png_file_name` | No | If not provided the .png graph name will be the last label_name from a `.csv` file provided |
| `--log-x` | No | Logarithmic x-axis | 
| `--log-y` | No | Logarithmic y-axis |

Usually, running the program to create a graph with `n` valid `.csv` files for different data follow the lines of:

```
python3 graph.py n*(.csv_file_path data_label) graph.png_file_name --log-x --log-y
```

Real example:
```bash
python3 graph.py jacobsonrank/results/ns_select0_log.csv select0logyay jacobsonrank/results/rank0_log.csv rankyay jacobsonrank/results/rank1_log.csv rank1yay jacobsonrank/results/select1_log.csv select1yay coolname --log-x --log-y
```

In addition, the `.csv` files are expected to be in the standard:

```csv
"Order";"Time"
10000;101.925
100000;114.84
1000000;139.694
10000000;206.787
100000000;306.412
```
So that the graphs output is better looking and has its axis properly defined.

If you run a command that does not follow the standards covered by this file, you should get an Error.

## Interquartile Range

Given an array of double values representing the average time of an arbitrary operation, the interquartile method:

- 1) Sorts the original array;
- 2) Divide it into 3 regions where the median of each subarray (Q1,Q2 and Q3) represents the whole;
- 3) Calculates the IQR as Q3 - Q1;
- 4) Defines the upper and lower boundaries in order to eliminate disruptive values;
- 5) Filters the original values and remove the ones that surpass the previous parameters;
- 6) Verifies if the valid array with the filtered parameters has enough elements such that represents the whole;
- 7) If not valid, makes so the program repeats the previous steps until `vi` is valid a maximum of times else not valid;

Our Extra validation:

- After the interquartile range verification, we calculate the mean and standard deviation of the valid array values;
- Given these variables, we can calculate the Coefficient Variation (standard deviation / mean) to assure an reliable value distribution;
- if this previous parameter isnt satisfatory, we proceed to run the tests all over again, just like in `vii`. 

Main function:

- `Statistics calculate_interquartil(const vector<double>& samples)`: Return a Statistics object

```
struct Statistics {
    double q1;
    double median; // q2
    double q3;
    double iqr;

    double lower_bound;
    double upper_bound;

    double mean;
    double standard_deviation;
    double coefficient_variation;

    vector<double> valid;
    vector<double> outliers;

    bool is_valid;
};
```

## Notes

For more informations, access [main README](../README.md).
