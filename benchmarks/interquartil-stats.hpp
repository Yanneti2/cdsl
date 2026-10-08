#include <vector>

using namespace std;

#ifndef INTERQUARTILE_RANGE
#define INTERQUARTILE_RANGE

// struct BenchmarkConfig {
//     int samples; 30
//     int warmup;  5
//     int max_attempts; 5

//     double max_outlier_ratio; 0.20
// };

// struct BenchmarkResults {
//     vector<double> samples;
//     vector<double> valid_samples;
//     vector<double> outliers;

//     double q1;
//     double q3;
//     double iqr;
// };

const double MAX_COEFFICIENT_VARIATION = 0.10;
const double MAX_OUTLIER_RATIO = 0.20;
const int MIN_WARMUP_SAMPLES = 10;
const int MIN_VALID_SAMPLES = 25; // 6/7 ?
const int MAX_ATTEMPTS = 5;
const int MIN_SAMPLES = 30; // -> 10?

struct Statistics {
    double q1;
    double median; // q2
    double q3;
    double iqr;

    double lower_bound;
    double upper_bound;

    double mean;
    double standard_deviation; // desvio padrao
    double coefficient_variation; // dp / media(mean)

    vector<double> valid;
    vector<double> outliers;

    bool is_valid;
};

double get_median(const vector<double>& arr);
double get_mean(const vector<double>& arr);
double get_standard_deviation(const vector<double>& arr, double mean);
Statistics calculate_interquartil(const vector<double>& samples);

#endif