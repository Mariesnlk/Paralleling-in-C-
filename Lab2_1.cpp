include <iostream>
#include <cassert>
#include <stdio.h>
#include <chrono>
#include <omp.h>
#define N 2000000
#define NTHREADS 4
#define NITERATIONS 100

using namespace std;

int getRandomNumber(int min, int max) {
    int num = min + rand() % (max - min + 1);
    return num;
}

int main(int argc, char** argv) {

    double *array = new double[N];
    double *new_array = new double[N];

    for (int i = 0; i < 3; i++)
        array[i] = getRandomNumber(0, 5);

    auto beginTime = chrono::steady_clock::now();

    omp_set_num_threads(NTHREADS);
    for (int i = 0; i < NITERATIONS; i++) {
            #pragma omp parallel for
            for (int j = 0; j < N; j++) {

                if (j == 0 || j == N - 1) {
                    if (array[j] == 0)
                        new_array[j] = array[j];
                    else
                        new_array[j] = array[j] / 2;
                }
                else {
                    if (array[j] == 0)
                        new_array[j] = array[j];
                    else
                        new_array[j] = (array[j - 1] + array[j + 1]) / 2;
                }

            }
   
    }

    auto endTime = chrono::steady_clock::now();
    auto resultTime = chrono::duration_cast<chrono::milliseconds>(endTime - beginTime).count();
    cout << "N : " << N << " Threads : " <<  NTHREADS << " Time : " << (double)resultTime / 1000 << endl;

    return 0;
}
