#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DAYS 1200
#define MAX_STOCKS 5
#define SIMULATIONS 100000

int main(){
    FILE *fp=fopen("/home/avishka/HPC/project/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv", "r"); 
    if(!fp){
        printf("Error in opening file");
        return 1;
    }

    //reading the header
    char line[4000];//character array to read and store one line from csv. temporary buffer
    char *token;
    int stocks=0;
    if(fgets(line, size_of(line), fp)){ //read first line
        token=strtok(line, ","); //split into tokens. ignore 'DATE'.
        while(token=(NULL,",")!=NULL){ // read other tokens/stocks in header
            stocks=stocks+1;
        }
        printf("No of stocks = %d\n", stocks);       
    }
    else{
    fclose(fp);
    return 1;
    }
    int day_count;

    double prices[MAX_DAYS][MAX_STOCKS];
    //read prices
    while(fgets(line, size_of(line), fp) && day_count<MAX_DAYS){
        token=strtok(line, ", "); //igone dates in first column
        for(int i=0;i<stocks;i++){
            token=strtok(NULL, ",");
            prices[day_count][i]=atof(token);// 
        }
        day_count=day_count+1;
    }
    fclose(fp);

    //calculate daily reurns
    double returns[MAX_DAYS-1][MAX_STOCKS];
    for(int i=1; i<day_count;i++){ //today - yesterday
        for(int j=0;j<stocks;j++){
            returns[i-1][j]=(prices[i-1][j]-prices[i][j])/(prices[i-1][j]); //ith day, jth company
        }
    }

    int return_days=day_count-1;

    //calculate mean return for eack stock j
    double mean_returns[MAX_STOCKS];
    for(int j=0;j<stocks;j++){
        double sum=0.0;
        for(int i=0;i<return_days;i++){
            sum=sum+returns[i][j];
        }
        mean_returns[j]=sum/return_days;
    }

    //
    double cov[MAX_STOCKS][MAX_STOCKS];
    for (int i = 0; i < stocks; i++) {
        for (int j = 0; j < stocks; j++) {
            double cov_ij = 0.0;
            for (int k = 0; k < return_days; k++) {
                cov_ij += (returns[k][i] - mean_returns[i]) * (returns[k][j] - mean_returns[j]);
            }
            cov[i][j] = cov_ij / (return_days - 1);
        }
    }

}

