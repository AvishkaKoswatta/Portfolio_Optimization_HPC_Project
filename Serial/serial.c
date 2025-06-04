#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_DAYS 1200
#define NUM_STOCKS 5

const char* filenames[] = { "AAPL", "AMZN", "GOOGL", "JPM", "MSFT" };

double prices[MAX_DAYS][NUM_STOCKS];
int num_days = 0;

// Calculate daily return
void compute_returns(double returns[MAX_DAYS - 1][NUM_STOCKS]) {
    for (int i = 1; i < num_days; i++) {
        for (int j = 0; j < NUM_STOCKS; j++) {
            returns[i - 1][j] = (prices[i][j] - prices[i - 1][j]) / prices[i - 1][j];
        }
    }
}

// Calculate mean return for each stock
void compute_mean(double returns[MAX_DAYS - 1][NUM_STOCKS], double mean[NUM_STOCKS]) {
    for (int j = 0; j < NUM_STOCKS; j++) {
        double sum = 0.0;
        for (int i = 0; i < num_days - 1; i++) {
            sum += returns[i][j];
        }
        mean[j] = sum / (num_days - 1);
    }
}

// Calculate standard deviation (risk)
void compute_stddev(double returns[MAX_DAYS - 1][NUM_STOCKS], double mean[NUM_STOCKS], double stddev[NUM_STOCKS]) {
    for (int j = 0; j < NUM_STOCKS; j++) {
        double sum_sq = 0.0;
        for (int i = 0; i < num_days - 1; i++) {
            double diff = returns[i][j] - mean[j];
            sum_sq += diff * diff;
        }
        stddev[j] = sqrt(sum_sq / (num_days - 2)); // sample std deviation
    }
}

int main() {
    FILE* file = fopen("prices.csv", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[1024];
    fgets(line, sizeof(line), file); // skip header

    while (fgets(line, sizeof(line), file)) {
        char* token = strtok(line, ","); // skip date
        for (int i = 0; i < NUM_STOCKS; i++) {
            token = strtok(NULL, ",");
            if (token) {
                prices[num_days][i] = atof(token);
            } else {
                prices[num_days][i] = 0.0;
            }
        }
        num_days++;
    }

    fclose(file);

    double returns[MAX_DAYS - 1][NUM_STOCKS];
    double mean[NUM_STOCKS], stddev[NUM_STOCKS];

    compute_returns(returns);
    compute_mean(returns, mean);
    compute_stddev(returns, mean, stddev);

    printf("Stock\tMean Return\tRisk (Std Dev)\n");
    for (int i = 0; i < NUM_STOCKS; i++) {
        printf("%s\t%.6f\t%.6f\n", filenames[i], mean[i], stddev[i]);
    }

    return 0;
}
