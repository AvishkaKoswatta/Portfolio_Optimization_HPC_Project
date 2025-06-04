#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

int main(){
    FILE *fp=fopen("/home/avishka/HPC/project/new/Portfolio_Optimization_HPC_Project/Data/new_all_stocks_5yr.csv","r");
    if(!fp){
        printf("Error opening file");
        return 1;
    }
}   