#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <time.h>

int getRandomNumber(int min, int max) {
    int num = min + rand() % (max - min + 1);
    return num;
}


int main() {
    int n;
    int nThreads;
    printf("Enter the size of the array : ");
    scanf_s("%d", &n);
    printf("Enter the size of the threads : ");
    scanf_s("%d", &nThreads);

    int* main = new int[n];
    int* sorted = new int[n];

    //create array with random numbers
    int i1;
    for (i1 = 0; i1 < n; i1++) {
        main[i1] = getRandomNumber(1, 100);
        sorted[i1] = 0;
    }

    //parallel
    int i, j, count;
    omp_set_num_threads(nThreads);
    double start_time = omp_get_wtime();
#pragma omp parallel private(i, j, count)
    {
#pragma omp for
        for (i = 0; i < n; i++) {
            count = 0;
            for (j = 0; j < n; j++) {
                if (main[i] > main[j])
                    count++;
            }
            while (sorted[count] != 0)
                count++;
            sorted[count] = main[i];
        }
    }
    double end_time = omp_get_wtime();
    double time_used = end_time - start_time;
    printf("Parallel time: %f s\n", time_used);

    delete[] main;
    delete[] sorted;


    return 0;
}
