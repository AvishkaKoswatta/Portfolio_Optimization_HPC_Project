#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX_STOCKS 5
#define MAX_DAYS 1200
#define SIMULATIONS 100000
#define RISK_FREE_RATE 0.01
#define TRADING_DAYS 252

void normalize_weights(double weights[], int n) {
    double sum = 0.0;
    for (int i = 0; i < n; i++) sum += weights[i];
    for (int i = 0; i < n; i++) weights[i] /= sum;
}

double portfolio_return(double weights[], double returns[], int n) {
    double port_return = 0.0;
    for (int i = 0; i < n; i++) port_return += weights[i] * returns[i];
    return port_return;
}

double portfolio_variance(double weights[], double cov[][MAX_STOCKS], int n) {
    double var = 0.0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            var += weights[i] * weights[j] * cov[i][j];
    return var;
}

int main() {
    FILE *fp = fopen("/home/avishka/HPC/project/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv", "r"); //open for read
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    char line[4096];
    char *token;
    int n_stocks = 0;

    double prices[MAX_DAYS][MAX_STOCKS];
    double returns[MAX_DAYS-1][MAX_STOCKS];
    int day_count = 0;
    char *stock_names[MAX_STOCKS] = {"AAPL", "AMZN", "GOOGL", "JPM", "MSFT"};


    // Read header
    if (fgets(line, sizeof(line), fp)) {
        // Expecting: date,AAPL,AMZN,GOOGL,JPM,MSFT
        token = strtok(line, ","); // "date"
        while ((token = strtok(NULL, ",")) != NULL) {
            n_stocks++;
            if (n_stocks > MAX_STOCKS) {
                printf("Too many stocks in header; max supported is %d\n", MAX_STOCKS);
                fclose(fp);
                return 1;
            }
        }
        printf("Number of stocks detected: %d\n", n_stocks);
    } else {
        printf("File is empty\n");
        fclose(fp);
        return 1;
    }

    // Read prices up to MAX_DAYS lines
    while (fgets(line, sizeof(line), fp) && day_count < MAX_DAYS) {
        token = strtok(line, ","); // date
        for (int i = 0; i < n_stocks; i++) {
            token = strtok(NULL, ",");
            if (!token) {
                printf("Missing data on line %d\n", day_count + 2);
                fclose(fp);
                return 1;
            }
            prices[day_count][i] = atof(token);
        }
        day_count++;
    }
    fclose(fp);

    printf("Read %d days of price data\n", day_count);

    if (day_count < 2) {
        printf("Not enough data to calculate returns\n");
        return 1;
    }

    // Calculate daily simple returns
    for (int i = 1; i < day_count; i++) {
        for (int j = 0; j < n_stocks; j++) {
            returns[i-1][j] = (prices[i][j] - prices[i-1][j]) / prices[i-1][j];
        }
    }
    int return_days = day_count - 1;

    // Calculate mean daily returns
    double mean_returns[MAX_STOCKS];
    for (int j = 0; j < n_stocks; j++) {
        double sum = 0.0;
        for (int i = 0; i < return_days; i++) sum += returns[i][j];
        mean_returns[j] = sum / return_days;
    }

    // Calculate covariance matrix of daily returns
    double cov[MAX_STOCKS][MAX_STOCKS];
    for (int i = 0; i < n_stocks; i++) {
        for (int j = 0; j < n_stocks; j++) {
            double cov_ij = 0.0;
            for (int k = 0; k < return_days; k++) {
                cov_ij += (returns[k][i] - mean_returns[i]) * (returns[k][j] - mean_returns[j]);
            }
            cov[i][j] = cov_ij / (return_days - 1);
        }
    }

    // Annualize mean returns and covariance matrix
    for (int i = 0; i < n_stocks; i++) {
        mean_returns[i] *= TRADING_DAYS;
        for (int j = 0; j < n_stocks; j++) {
            cov[i][j] *= TRADING_DAYS;
        }
    }

    // Monte Carlo portfolio simulation
    srand(time(NULL));

    double best_min_risk = 1e9;
    double best_max_return = -1e9;
    double best_max_sharpe = -1e9;

    double weights_min_risk[MAX_STOCKS];
    double weights_max_return[MAX_STOCKS];
    double weights_max_sharpe[MAX_STOCKS];

    for (int sim = 0; sim < SIMULATIONS; sim++) {
        double weights[MAX_STOCKS];
        for (int i = 0; i < n_stocks; i++)
            weights[i] = (double)rand() / RAND_MAX;

        normalize_weights(weights, n_stocks);

        double ret = portfolio_return(weights, mean_returns, n_stocks);
        double var = portfolio_variance(weights, cov, n_stocks);
        double risk = sqrt(var);

        double sharpe = (ret - RISK_FREE_RATE) / risk;

        if (risk < best_min_risk) {
            best_min_risk = risk;
            memcpy(weights_min_risk, weights, sizeof(weights));
        }

        if (ret > best_max_return) {
            best_max_return = ret;
            memcpy(weights_max_return, weights, sizeof(weights));
        }

        if (sharpe > best_max_sharpe) {
            best_max_sharpe = sharpe;
            memcpy(weights_max_sharpe, weights, sizeof(weights));
        }
    }

    printf("\nMinimum Risk Portfolio:\n");
    for (int i = 0; i < n_stocks; i++)
        printf("Stock %d weight: %.4f\n", i+1, weights_min_risk[i]);
    printf("Risk (Std Dev): %.4f\n", best_min_risk);

    printf("\nMaximum Return Portfolio:\n");
    for (int i = 0; i < n_stocks; i++)
        printf("Stock %d weight: %.4f\n", i+1, weights_max_return[i]);
    printf("Return: %.4f\n", best_max_return);

    // printf("\nMaximum Sharpe Ratio Portfolio:\n");
    // for (int i = 0; i < n_stocks; i++)
    //     printf("Stock %d weight: %.4f\n", i+1, weights_max_sharpe[i]);
    // printf("Sharpe Ratio: %.4f\n", best_max_sharpe);

    printf("\nMaximum Sharpe Ratio Portfolio Allocation:\n");
for (int i = 0; i < n_stocks; i++) {
    printf("%-6s: %.2f%%\n", stock_names[i], weights_max_sharpe[i] * 100);
}
// printf("Sharpe Ratio: %.4f\n", best_max_sharpe);


    return 0;
}
