#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPSILON 0.001
#define INF 1.0/0.0

void kmeans(int maxIter, char *inputFile, char *outputFile);
double **getDataFromFile(char *filename);
void getDimensions(char *str);
int getMinClusterIndex(double *datapoint, double **kmeansList);
double calcDistance(double *x, double *u);
int updateKmeans(double **kmeansList, double **clusterSumList, int *clusterSizeList);
void writeToFile(char *filename, double **kmeansList);
void throwError(int errorCode);

int dim;
int numOfRows;
int k;
int main(int argc, char **argv){
    char *inputFile, *outputFile;
    int maxIterations = 200, e;
    if (argc == 4)
    {
        e = sscanf(argv[1], "%d", &k);
        if (e != 1)
        {
            throwError(0);
        }
        inputFile = argv[2];
        outputFile = argv[3];
    }
    else if (argc == 5)
    {
        e = sscanf(argv[1], "%d", &k);
        if (e != 1)
        {
            throwError(0);
        }
        e = sscanf(argv[2], "%d", &maxIterations);
        if (e != 1)
        {
            throwError(0);
        }
        inputFile = argv[3];
        outputFile = argv[4];
    }
    else
    {
        throwError(0);
    }
    kmeans(maxIterations, inputFile, outputFile);
    exit(0);
}

void kmeans(int maxIter, char *inputFile, char *outputFile){
    double **datapoints, **kmeansList, **clusterSumList;
    double *datapoint;
    int *clusterSizeList;
    int curIter, isAnyMoreThanEpsilon, minClusterIndex, i, j;
    getDimensions(inputFile);
    datapoints = getDataFromFile(inputFile);
    kmeansList = calloc(k, sizeof(double*));
    for (i = 0; i < k; i++)
    {
        kmeansList[i] = calloc(dim, sizeof(double));
        for (j = 0; j < dim; j++)
        {
            kmeansList[i][j] = datapoints[i][j];
        }
    }
    curIter = 0;
    isAnyMoreThanEpsilon = 1;
    while (isAnyMoreThanEpsilon && curIter < maxIter)
    {
        curIter++;
        clusterSumList = calloc(k, sizeof(double*));
        for (i = 0; i < k; i++)
        {
            clusterSumList[i] = calloc(dim, sizeof(double));
        }
        clusterSizeList = calloc(k, sizeof(int));
        for (i = 0; i < numOfRows; i++)
        {
            datapoint = datapoints[i];
            minClusterIndex = getMinClusterIndex(datapoint, kmeansList);
            clusterSizeList[minClusterIndex]++;
            for (j = 0; j < dim; j++)
            {
                clusterSumList[minClusterIndex][j] += datapoint[j];
            }
        }
        isAnyMoreThanEpsilon = updateKmeans(kmeansList, clusterSumList, clusterSizeList);
    }
    writeToFile(outputFile, kmeansList);
    for (i = 0; i < numOfRows; i++)
    {
        free(datapoints[i]);
        if (i < k)
        {
            free(kmeansList[i]);
            free(clusterSumList[i]);
        }
    }
    free(kmeansList);
    free(datapoints);
    free(clusterSumList);
    free(clusterSizeList);
}

double **getDataFromFile(char *filename){
    double **datapoints;
    int e, i, j;
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        throwError(1);
    }
    datapoints = calloc(numOfRows, sizeof(double*));
    for (i = 0; i < numOfRows; i++)
    {
        datapoints[i] = calloc(dim, sizeof(double));
        for (j = 0; j < dim; j++)
        {
            e = fscanf(file, "%lf", &datapoints[i][j]);
            fgetc(file);
            if (e != 1)
            {
                throwError(1);
            }
        }
    }
    fclose(file);
    return datapoints;
}

void getDimensions(char *filename){
    double num;
    int numOfNums = 0, e;
    char c;
    FILE *file = fopen(filename, "r");
    dim = -1;
    if (file == NULL)
    {
        throwError(1);
    }
    while (!feof(file))
    {
        e = fscanf(file, "%lf", &num);
        c = fgetc(file);    
        if (e == 1)
        {
            numOfNums++;
        }
        if (dim == -1 && c == '\n')
        {
            dim = numOfNums;
        }
    }
    fclose(file);
    numOfRows = numOfNums / dim;
}

int getMinClusterIndex(double *datapoint, double **kmeansList){
    double minDist = INF, curDistance;
    int minClusterIndex = -1, i;
    for (i = 0; i < k; i++)
    {
        curDistance = calcDistance(datapoint, kmeansList[i]);
        if (curDistance < minDist)
        {
            minDist = curDistance;
            minClusterIndex = i;
        }
    }
    return minClusterIndex;    
}

double calcDistance(double *x, double *u){
    double sumSquare = 0;
    int i;
    for (i = 0; i < dim; i++)
    {
        sumSquare += pow(x[i] - u[i], 2);
    }
    return sqrt(sumSquare);
}

int updateKmeans(double **kmeansList, double **clusterSumList, int *clusterSizeList){
    double* prevKmean;
    int isAnyMoreThanEpsilon = 0, i, j;
    prevKmean = calloc(dim, sizeof(double));
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < dim; j++)
        {
            prevKmean[j] = kmeansList[i][j];
            clusterSumList[i][j] /= clusterSizeList[i];
        }
        for (j = 0; j < dim; j++)
        {
            kmeansList[i][j] = clusterSumList[i][j];
        }
        if (calcDistance(kmeansList[i], prevKmean) >= EPSILON)
        {
            isAnyMoreThanEpsilon = 1;
        }
    }
    free(prevKmean);
    return isAnyMoreThanEpsilon;
}

void writeToFile(char *filename, double **kmeansList){
    int i, j;
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        throwError(1);
    }
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < dim; j++)
        {
            fprintf(file, "%.4f", kmeansList[i][j]);
            if (j < dim-1)
            {
                fprintf(file, ",");
            }
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void throwError(int errorCode){
    switch (errorCode)
    {
    case 0:
        printf("Invalid Input!");
        break;
    case 1:
        printf("An Error Has Occured");
        break;
    }
    exit(1);
}