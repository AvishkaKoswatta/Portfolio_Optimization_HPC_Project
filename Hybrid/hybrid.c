#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>
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

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = MPI_Wtime();

    FILE *fp;
    char line[4096], *token;
    double prices[MAX_DAYS][MAX_STOCKS], returns[MAX_DAYS - 1][MAX_STOCKS];
    int day_count = 0, n_stocks = 0;

    if (rank == 0) {
        fp = fopen("/home/avishka/HPC/project/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv", "r");
        if (!fp) {
            printf("Error opening file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        if (fgets(line, sizeof(line), fp)) {
            token = strtok(line, ",");
            while ((token = strtok(NULL, ",")) != NULL) {
                n_stocks++;
                if (n_stocks > MAX_STOCKS) {
                    printf("Too many stocks\n");
                    fclose(fp);
                    MPI_Abort(MPI_COMM_WORLD, 1);
                }
            }
        }

        while (fgets(line, sizeof(line), fp) && day_count < MAX_DAYS) {
            token = strtok(line, ",");
            for (int i = 0; i < n_stocks; i++) {
                token = strtok(NULL, ",");
                prices[day_count][i] = atof(token);
            }
            day_count++;
        }
        fclose(fp);
    }

    MPI_Bcast(&n_stocks, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&day_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(prices, MAX_DAYS * MAX_STOCKS, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int return_days = day_count - 1;
    for (int i = 1; i < day_count; i++)
        for (int j = 0; j < n_stocks; j++)
            returns[i - 1][j] = (prices[i][j] - prices[i - 1][j]) / prices[i - 1][j];

    double mean_returns[MAX_STOCKS] = {0};
    for (int j = 0; j < n_stocks; j++) {
        for (int i = 0; i < return_days; i++)
            mean_returns[j] += returns[i][j];
        mean_returns[j] /= return_days;
    }

    double cov[MAX_STOCKS][MAX_STOCKS] = {0};
    for (int i = 0; i < n_stocks; i++) {
        for (int j = 0; j < n_stocks; j++) {
            for (int k = 0; k < return_days; k++) {
                cov[i][j] += (returns[k][i] - mean_returns[i]) * (returns[k][j] - mean_returns[j]);
            }
            cov[i][j] /= (return_days - 1);
        }
    }

    for (int i = 0; i < n_stocks; i++) {
        mean_returns[i] *= TRADING_DAYS;
        for (int j = 0; j < n_stocks; j++)
            cov[i][j] *= TRADING_DAYS;
    }

    int sims_per_process = SIMULATIONS / size;
    unsigned int seed = time(NULL) + rank * 100;

    double local_best_sharpe = -1e9;
    double local_weights_sharpe[MAX_STOCKS];

    #pragma omp parallel
    {
        unsigned int thread_seed = seed ^ omp_get_thread_num();
        double thread_best_sharpe = -1e9;
        double thread_weights[MAX_STOCKS];

        #pragma omp for
        for (int sim = 0; sim < sims_per_process; sim++) {
            double weights[MAX_STOCKS];
            for (int i = 0; i < n_stocks; i++)
                weights[i] = (double)rand_r(&thread_seed) / RAND_MAX;

            normalize_weights(weights, n_stocks);
            double ret = portfolio_return(weights, mean_returns, n_stocks);
            double risk = sqrt(portfolio_variance(weights, cov, n_stocks));
            double sharpe = (ret - RISK_FREE_RATE) / risk;

            if (sharpe > thread_best_sharpe) {
                thread_best_sharpe = sharpe;
                memcpy(thread_weights, weights, sizeof(weights));
            }
        }

        #pragma omp critical
        {
            if (thread_best_sharpe > local_best_sharpe) {
                local_best_sharpe = thread_best_sharpe;
                memcpy(local_weights_sharpe, thread_weights, sizeof(thread_weights));
            }
        }
    }

    struct {
        double value;
        int rank;
    } local_sharpe = {local_best_sharpe, rank}, best_sharpe;

    MPI_Allreduce(&local_sharpe, &best_sharpe, 1, MPI_DOUBLE_INT, MPI_MAXLOC, MPI_COMM_WORLD);
    MPI_Bcast(local_weights_sharpe, MAX_STOCKS, MPI_DOUBLE, best_sharpe.rank, MPI_COMM_WORLD);

    if (rank == 0) {
        char *stock_names[MAX_STOCKS] = {"APPLE", "AMAZON", "GOOGLE", "JPMorgan", "MICROSOFT"};
        printf("\nMaximum Sharpe Ratio Portfolio Allocation:\n");
        for (int i = 0; i < n_stocks; i++)
            printf("%-8s: %.2f%%\n", stock_names[i], local_weights_sharpe[i] * 100);

        double end_time = MPI_Wtime();
        printf("\nTotal Execution Time (Hybrid MPI + OpenMP): %.6f seconds\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
