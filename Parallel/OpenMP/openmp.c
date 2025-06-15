// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>
// #include <time.h>
// #include <omp.h>

// #define MAX_STOCKS 10
// #define MAX_DAYS 1500
// #define SIMULATIONS 100000
// #define RISK_FREE_RATE 0.01

// char stock_names[MAX_STOCKS][10];
// double prices[MAX_DAYS][MAX_STOCKS];
// double returns[MAX_DAYS - 1][MAX_STOCKS];
// double mean_returns[MAX_STOCKS];
// double cov[MAX_STOCKS][MAX_STOCKS];

// int n_stocks = 0;
// int n_days = 0;

// void read_csv(const char *filename) {
//     FILE *fp = fopen(filename, "r");
//     if (!fp) {
//         perror("Error opening file");
//         exit(EXIT_FAILURE);
//     }

//     char line[1024];
//     fgets(line, sizeof(line), fp); 

//     char *token = strtok(line, ",\n");
//     token = strtok(NULL, ",\n"); 
//     while (token && n_stocks < MAX_STOCKS) {
//         strncpy(stock_names[n_stocks++], token, 10);
//         token = strtok(NULL, ",\n");
//     }

//     while (fgets(line, sizeof(line), fp) && n_days < 1200) {
//         token = strtok(line, ",\n");
//         for (int i = 0; i < n_stocks; i++) {
//             token = strtok(NULL, ",\n");
//             prices[n_days][i] = atof(token);
//         }
//         n_days++;
//     }
//     fclose(fp);
// }

// void calculate_returns() {
//     for (int i = 0; i < n_days - 1; i++) {
//         for (int j = 0; j < n_stocks; j++) {
//             returns[i][j] = (prices[i + 1][j] - prices[i][j]) / prices[i][j];
//         }
//     }
// }

// void calculate_mean_returns() {
//     for (int i = 0; i < n_stocks; i++) {
//         double sum = 0.0;
//         for (int j = 0; j < n_days - 1; j++) {
//             sum += returns[j][i];
//         }
//         mean_returns[i] = sum / (n_days - 1);
//     }
// }

// void calculate_covariance_matrix() {
//     for (int i = 0; i < n_stocks; i++) {
//         for (int j = 0; j < n_stocks; j++) {
//             double sum = 0.0;
//             for (int k = 0; k < n_days - 1; k++) {
//                 sum += (returns[k][i] - mean_returns[i]) * (returns[k][j] - mean_returns[j]);
//             }
//             cov[i][j] = sum / (n_days - 2);
//         }
//     }
// }

// void normalize_weights(double *weights, int n) {
//     double sum = 0.0;
//     for (int i = 0; i < n; i++) sum += weights[i];
//     for (int i = 0; i < n; i++) weights[i] /= sum;
// }

// double portfolio_return(double *weights, double *means, int n) {
//     double result = 0.0;
//     for (int i = 0; i < n; i++) result += weights[i] * means[i];
//     return result;
// }

// double portfolio_variance(double *weights, double cov[MAX_STOCKS][MAX_STOCKS], int n) {
//     double var = 0.0;
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             var += weights[i] * weights[j] * cov[i][j];
//         }
//     }
//     return var;
// }

// int main() {
//     clock_t start = clock();
//     read_csv("/home/avishka/HPC/project/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv");
//     printf("Number of stocks detected: %d\n", n_stocks);
//     printf("Read %d days of price data\n", n_days);

//     calculate_returns();
//     calculate_mean_returns();
//     calculate_covariance_matrix();

//     double best_min_risk = 1e9, best_max_return = -1e9;
//     double weights_min_risk[MAX_STOCKS] = {0};
//     double weights_max_return[MAX_STOCKS] = {0};

//     #pragma omp parallel
//     {
//         unsigned int seed = (unsigned int)time(NULL) ^ omp_get_thread_num();
//         double local_weights[MAX_STOCKS];
//         double local_best_min_risk = 1e9, local_best_max_return = -1e9;
//         double local_min_risk_weights[MAX_STOCKS];
//         double local_max_return_weights[MAX_STOCKS];

//         #pragma omp for
//         for (int sim = 0; sim < SIMULATIONS; sim++) {
//             for (int i = 0; i < n_stocks; i++)
//                 local_weights[i] = (double)rand_r(&seed) / RAND_MAX;

//             normalize_weights(local_weights, n_stocks);

//             double ret = portfolio_return(local_weights, mean_returns, n_stocks);
//             double risk = sqrt(portfolio_variance(local_weights, cov, n_stocks));

//             if (risk < local_best_min_risk) {
//                 local_best_min_risk = risk;
//                 memcpy(local_min_risk_weights, local_weights, sizeof(local_min_risk_weights));
//             }

//             if (ret > local_best_max_return) {
//                 local_best_max_return = ret;
//                 memcpy(local_max_return_weights, local_weights, sizeof(local_max_return_weights));
//             }
//         }

//         #pragma omp critical
//         {
//             if (local_best_min_risk < best_min_risk) {
//                 best_min_risk = local_best_min_risk;
//                 memcpy(weights_min_risk, local_min_risk_weights, sizeof(weights_min_risk));
//             }
//             if (local_best_max_return > best_max_return) {
//                 best_max_return = local_best_max_return;
//                 memcpy(weights_max_return, local_max_return_weights, sizeof(weights_max_return));
//             }
//         }
//     }

//     printf("\nMinimum Risk Portfolio:\n");
//     for (int i = 0; i < n_stocks; i++)
//         printf("Stock %d weight: %.4f\n", i + 1, weights_min_risk[i]);
//     printf("Risk (Std Dev): %.4f\n", best_min_risk);

//     printf("\nMaximum Return Portfolio:\n");
//     for (int i = 0; i < n_stocks; i++)
//         printf("Stock %d weight: %.4f\n", i + 1, weights_max_return[i]);
//     printf("Return: %.4f\n", best_max_return);

//     clock_t end = clock();
//     double total_time = (double)(end - start) / CLOCKS_PER_SEC;
//     printf("\nExecution Time: %.4f seconds\n", total_time);
//     return 0;

    

// }




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <omp.h>

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
    FILE *fp = fopen("/home/avishka/HPC/project/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv", "r");
    if (!fp) {
        printf("Error opening file\n");
        return 1;
    }

    char line[4096];
    char *token;
    int n_stocks = 0;

    double prices[MAX_DAYS][MAX_STOCKS];
    double returns[MAX_DAYS - 1][MAX_STOCKS];
    int day_count = 0;

    // Read header
    if (fgets(line, sizeof(line), fp)) {
        token = strtok(line, ","); // Skip "date"
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

    while (fgets(line, sizeof(line), fp) && day_count < MAX_DAYS) {
        token = strtok(line, ",");
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

    for (int i = 1; i < day_count; i++) {
        for (int j = 0; j < n_stocks; j++) {
            returns[i - 1][j] = (prices[i][j] - prices[i - 1][j]) / prices[i - 1][j];
        }
    }
    int return_days = day_count - 1;

    double mean_returns[MAX_STOCKS];
    for (int j = 0; j < n_stocks; j++) {
        double sum = 0.0;
        for (int i = 0; i < return_days; i++) sum += returns[i][j];
        mean_returns[j] = sum / return_days;
    }

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

    for (int i = 0; i < n_stocks; i++) {
        mean_returns[i] *= TRADING_DAYS;
        for (int j = 0; j < n_stocks; j++) {
            cov[i][j] *= TRADING_DAYS;
        }
    }

    char *stock_names[MAX_STOCKS] = {"APPLE", "AMAZON", "GOOGLE", "JPMorgan", "MICROSOFT"};

    double best_min_risk = 1e9;
    double best_max_return = -1e9;
    double best_max_sharpe = -1e9;

    double weights_min_risk[MAX_STOCKS];
    double weights_max_return[MAX_STOCKS];
    double weights_max_sharpe[MAX_STOCKS];

    #pragma omp parallel
    {
        double local_best_min_risk = 1e9;
        double local_best_max_return = -1e9;
        double local_best_max_sharpe = -1e9;

        double local_weights_min_risk[MAX_STOCKS];
        double local_weights_max_return[MAX_STOCKS];
        double local_weights_max_sharpe[MAX_STOCKS];

        unsigned int seed = time(NULL) ^ omp_get_thread_num();

        #pragma omp for
        for (int sim = 0; sim < SIMULATIONS; sim++) {
            double weights[MAX_STOCKS];
            for (int i = 0; i < n_stocks; i++)
                weights[i] = (double)rand_r(&seed) / RAND_MAX;

            normalize_weights(weights, n_stocks);

            double ret = portfolio_return(weights, mean_returns, n_stocks);
            double var = portfolio_variance(weights, cov, n_stocks);
            double risk = sqrt(var);
            double sharpe = (ret - RISK_FREE_RATE) / risk;

            if (risk < local_best_min_risk) {
                local_best_min_risk = risk;
                memcpy(local_weights_min_risk, weights, sizeof(weights));
            }
            if (ret > local_best_max_return) {
                local_best_max_return = ret;
                memcpy(local_weights_max_return, weights, sizeof(weights));
            }
            if (sharpe > local_best_max_sharpe) {
                local_best_max_sharpe = sharpe;
                memcpy(local_weights_max_sharpe, weights, sizeof(weights));
            }
        }

        #pragma omp critical
        {
            if (local_best_min_risk < best_min_risk) {
                best_min_risk = local_best_min_risk;
                memcpy(weights_min_risk, local_weights_min_risk, sizeof(weights_min_risk));
            }
            if (local_best_max_return > best_max_return) {
                best_max_return = local_best_max_return;
                memcpy(weights_max_return, local_weights_max_return, sizeof(weights_max_return));
            }
            if (local_best_max_sharpe > best_max_sharpe) {
                best_max_sharpe = local_best_max_sharpe;
                memcpy(weights_max_sharpe, local_weights_max_sharpe, sizeof(weights_max_sharpe));
            }
        }
    }

    printf("\nMaximum Sharpe Ratio Portfolio Allocation:\n");
    for (int i = 0; i < n_stocks; i++) {
        printf("%-6s: %.2f%%\n", stock_names[i], weights_max_sharpe[i] * 100);
    }
    // printf("Sharpe Ratio: %.4f\n", best_max_sharpe);

    return 0;
}
