#include <optional>
#include <vector>

using namespace std;

#ifndef INTERQUARTILE_RANGE
#define INTERQUARTILE_RANGE

const int SIMPLE_REPETITIONS = 10;
const int OPERATIONS = 100000;

struct StatsConfig {
    double max_coefficient_variation = 0.10;
    double max_outlier_ratio = 0.20;
    int min_warmup_samples = 10;
    int min_valid_samples = 25;
    int max_attempts = 5;
    int min_samples = 30;
};

struct Statistics {
    double q1;
    double median; // q2
    double q3;
    double iqr;

    double lower_bound;
    double upper_bound;

    double mean;
    double standard_deviation; // desvio padrao
    double coefficient_variation; // (dp / media(mean))

    vector<double> valid;
    vector<double> outliers;

    bool is_valid;
};

struct BenchmarkResult {
    double time;
    optional<Statistics> stats;
};

double get_median(const vector<double>& arr);
double get_mean(const vector<double>& arr);
double get_standard_deviation(const vector<double>& arr, double mean);
Statistics calculate_interquartil(const vector<double>& original_samples, const StatsConfig& config);

#endif