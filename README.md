## Portfolio optimization using Monte Carlo simulation in C

This project performs portfolio optimization using the Monte Carlo simulation technique. It aims to identify the optimal allocation of assets (stocks) that minimizes risk, maximizes return, or maximizes the Sharpe ratio.

---

## 1. About the Serial Version

- **Number of Stocks**: 5  
  (Apple - AAPL, Amazon - AMZN, Google - GOOGL, JPMorgan - JPM, Microsoft - MSFT)

- **Historical Data**: 5 years of daily closing prices (approximately 1200 trading days)

- **Data Format**: CSV file with structure  
  `date,AAPL,AMZN,GOOGL,JPM,MSFT`

- **Simulation Count**: 100,000 randomly generated portfolios

---

## 2. Definitions and Concepts

### Monte Carlo Simulation

A statistical technique used to estimate the outcome of uncertain processes by simulating random inputs. In this context, it is used to generate thousands of random portfolio weight combinations and evaluate their performance based on return, risk, and Sharpe ratio.

### Daily Return

The percentage change in price between two consecutive trading days:

```
Return = (Price_today - Price_yesterday) / Price_yesterday
```

### Mean Return

The average of all daily returns over the historical period for a given stock.

### Covariance

A measure of how two stocks move together. It is used to assess the diversification benefit of combining assets.

### Portfolio Return

The weighted average of the mean returns of the assets in the portfolio:

```
Portfolio Return = ∑ (weight_i × mean_return_i)
```

### Portfolio Risk (Volatility)

The standard deviation of the portfolio’s returns, derived from the covariance matrix:

```
Portfolio Variance = ∑∑ (weight_i × weight_j × covariance_ij)
Portfolio Risk = sqrt(Portfolio Variance)
```

### Sharpe Ratio

A measure of risk-adjusted return:

```
Sharpe Ratio = (Portfolio Return - Risk-Free Rate) / Portfolio Risk
```

---

## 3. Monte Carlo Simulation Applied

For each simulation:
1. Random weights are generated for the 5 stocks.
2. The weights are normalized to ensure their sum equals 1.
3. The portfolio return, risk, and Sharpe ratio are calculated using:
   - Mean returns
   - Covariance matrix
   - Risk-free rate (fixed at 0.01)
4. The portfolio is evaluated:
   - If it has the lowest risk seen so far, it is stored as the **minimum risk portfolio**.
   - If it has the highest return, it is stored as the **maximum return portfolio**.
   - If it has the highest Sharpe ratio, it is stored as the **optimal Sharpe portfolio**.

After 100,000 simulations, the best portfolios for each criterion are reported.

---

## 4. How to Run the Code

### Prerequisites

- GCC compiler
- C standard libraries (math, stdlib, stdio, string, time)

### Steps

1. **Place CSV file** with the historical stock data in the `Data/` directory.  

2. **Compile the code:**
   ```bash
   gcc serial.c -o serial -lm
   ```

3. **Run the program:**
   ```bash
   ./seral
   ```

---

## 2. About the OpenMP(Parallel) Version
gcc -fopenmp openmp.c -o openmp -lm
./openmp





python3 app.py

gcc serial.c -o /home/avishka/HPC/project/portfolio/Portfolio_Optimization_HPC_Project/executables/serial -lm

mpicc mpi.c -o /home/avishka/HPC/project/portfolio/Portfolio_Optimization_HPC_Project/executables/mpi -lm

gcc -fopenmp openmp.c -o /home/avishka/HPC/project/portfolio Portfolio_Optimization_HPC_Project/executables/openmp -lm

mpicc -fopenmp hybrid.c -o /home/avishka/HPC/project/portfolio Portfolio_Optimization_HPC_Project/executables/hybrid -lm