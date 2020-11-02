#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define MAXSIZE 10000
#define MAXWORKERS 10

static double read_timer() {

    static bool initialized = false;

    static struct timeval start;

    struct timeval end;

    if( !initialized ){

        gettimeofday( &start, NULL );

        initialized = true;
    }

    gettimeofday( &end, NULL );

    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int workers;

int sz;

int matrix[MAXSIZE][MAXSIZE];

int main(int argc, char *argv[]) {

  double sTime, rTime;

    int i, j, total=0;

    sz = (argc > 1) ? atoi(argv[1]) : MAXSIZE;

    workers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;

    if (sz > MAXSIZE) sz = MAXSIZE;

    if (workers > MAXWORKERS) workers = MAXWORKERS;

    //omp_set_num_threads(workers);

    for (i = 0; i < sz; i++) for (j = 0; j < sz; j++)matrix[i][j] = rand()%99;

    int min = matrix[0][0];

    int max = min;

    int minR, minC, maxR, maxC;

    sTime = read_timer();

    //reduction prevents data race for a variable and its operator, when declared private each thread has its own local copy
    #pragma omp parallel for reduction (+:total) private(j)

    printf("i am parallelised! \n");
    for (i = 0; i < sz; i++)
        for (j = 0; j < sz; j++){
            total += matrix[i][j];
            if (matrix[i][j] < min){
              //specifies the code block to be executed by only 1 thread at a time
#pragma omp critical
                {
                    if (matrix[i][j] < min) {
                        min = matrix[i][j];
                        minR = i;
                        minC = j;
                    }
                }
            }
            if (matrix[i][j] > max) {
#pragma omp critical
                {
                    if (matrix[i][j] > max) {
                        max = matrix[i][j];
                        maxR = i;
                        maxC = j;
                    }
                }
            }
        }
    rTime = read_timer();

        printf("MIN IS: %d AT: (%d,%d)\n", min, minR,minC);

        printf("MAX IS: %d AT: (%d,%d)\n", max, maxR,maxC);

        printf("SUM:  %d\n", total);

        printf("TIME WAS: %g seconds\n", rTime - sTime);
}
