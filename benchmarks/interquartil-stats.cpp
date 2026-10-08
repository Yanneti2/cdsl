#include "interquartil-stats.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

/*
    Q1 = mediana da metade inferior = primeiro quartil
    Q3 = mediana da metade superior = terceiro quartil
    IQR = Q3 - Q1

    lower = Q1 - 1.5 * IQR
    upper = Q3 + 1.5 * IQR

    tempo < lower  -> outlier
    tempo > upper  -> outlier

    if valid return, else run benchmark again until x

    x = max number of reiterations

    desvio padrao / media < 5% idealmente
*/

double get_median(const vector<double>& arr)
{
    size_t size = arr.size();
    size_t mid = size/2;

    if (size % 2 != 0) {
        return arr[size/2];
    } else {
        return (arr[size/2 - 1] + arr[size/2]) / 2.0;
    }
}

double get_mean(const vector<double>& arr)
{
    double mean = 0.0;

    for(double s : arr)
        mean += s;

    return (mean/arr.size());
}

double get_standard_deviation(const vector<double>& arr, double mean)
{
    double variance = 0.0;

    for (double s: arr)
        variance += (s - mean)*(s-mean);

    variance /= (arr.size() - 1);

    return sqrt(variance);
}

Statistics calculate_interquartil(const vector<double>& original_samples)
{
    bool is_valid = true;
    
    vector<double> samples = original_samples;
    sort(samples.begin(), samples.end());

    unsigned int sample_size = samples.size();

    double q2 = get_median(samples);

    vector<double> fh;
    vector<double> sh;

    for (unsigned int i = 0; i < sample_size/2; i++) fh.push_back(samples[i]);
    if (sample_size % 2 == 0)
    {
        for (unsigned int i = (sample_size/2); i < sample_size; i++) sh.push_back(samples[i]);
    } else {
        for (unsigned int i = (sample_size/2) + 1; i < sample_size; i++) sh.push_back(samples[i]);
    }

    double q1 = get_median(fh);
    double q3 = get_median(sh);
    double iqr = q3 - q1;

    double lower = q1 - (1.5 * iqr);
    double upper = q3 + (1.5 * iqr);

    vector<double> valid; // acceptable samples that not cross the lower and upper boundaries
    vector<double> outliers;

    for (double sample : samples)
    {
        if (sample > upper || sample < lower) outliers.push_back(sample);
        else valid.push_back(sample);
    }

    if ((double)outliers.size()/sample_size > MAX_OUTLIER_RATIO || valid.size() < MIN_VALID_SAMPLES)
        is_valid = false;

    // if valid.size() <= 0 problem with sd assigning

    double mean = get_mean(valid);
    double sd = get_standard_deviation(valid,mean);
    double cv = sd/mean;
    
    if (cv > MAX_COEFFICIENT_VARIATION) is_valid = false;

    Statistics res = {
        q1,
        q2,
        q3,
        iqr,
        lower,
        upper,
        mean,
        sd,
        cv,
        valid,
        outliers,
        is_valid
    };

    return res;
}