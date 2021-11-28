#include <iostream>
#include <cassert>
#include <stdio.h>
#include <chrono>
#include <omp.h>
#define THREADS 2

using namespace std;

int getRandomNumber(int min, int max) {
    int num = min + rand() % (max - min + 1);
    return num;
}

int main() {
    int I, m, n, k;
    cin >> I;
    cin >> m >> n;
    
    //create matrix
    double** matrix = new double* [m];
    for (int i = 0; i < m; ++i)
        matrix[i] = new double[n];

    //fill the matrix will 0 value
    for (int i = 0; i < m; i++) 
        for (int j = 0; j < n; j++)
            matrix[i][j] = 0;

    //create result matrix
    double** matrixResult = new double* [m];
    for (int i = 0; i < m; ++i)
        matrixResult[i] = new double[n];

    //fill the result matrix will 0 value
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            matrixResult[i][j] = 0;

    cin >> k;

    int x, y;
    double V;

    //fill the matrix in positions (x,y) value V
    for (int i = 0; i < k; ++i) {
        //cin >> x >> y >> V;
        x = getRandomNumber(m-1, n-1);
        y = getRandomNumber(m-1, n-1);
        V = getRandomNumber(1, 100);
        matrix[x][y] = V;
        matrixResult[x][y] = V;
    }

    /*cout << "-------Matrix-------" << endl;
    //print the matrix
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }*/

    auto beginTime = chrono::steady_clock::now();

    //clock_t start = clock();

    int i, j;
    for (int iter = 0; iter < I; iter++) {
#pragma omp parallel for num_threads(THREADS) private(i, j)
        for (i = 0; i < m; i++) {
            for (j = 0; j < n; j++) {
                if (matrix[i][j] == 0) {

                    //one row
                    if (m == 1) {
                        if (j == 0) {
                            matrixResult[i][j] = matrix[i][j];
                        }
                        else if (j == n - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1]) / 2;
                        }
                        else {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1] + matrix[i][j + 1]) / 3;
                        }
                    }
                    //one column
                    else if (n == 1) {
                        if (i == 0) {
                            matrixResult[i][j] = matrix[i][j];
                        }
                        else if (i == m - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i - 1][j]) / 2;
                        }
                        else {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i - 1][j] + matrix[i + 1][j]) / 3;
                        }
                    }
                    else {
                        if (i == 0 && j == 0) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j + 1] + matrix[i + 1][j]) / 3;
                        }
                        else if (i == 0 && j == n - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1] + matrix[i + 1][j]) / 3;
                        }
                        else if (i == m - 1 && j == 0) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i-1][j] + matrix[i][j + 1]) / 3;
                        }
                        else if (i == m - 1 && j == n - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1] + matrix[i - 1][j]) / 3;
                        }
                        else if (i == 0) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1] + matrix[i][j + 1] + matrix[i + 1][j]) / 4;
                        }
                        else if (i == m - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i - 1][j] + matrix[i][j + 1] + matrix[i][j - 1]) / 4;
                        }
                        else if (j == 0) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i][j - 1] + matrix[i][j + 1] + matrix[i + 1][j]) / 4;
                        }
                        else if (j == n - 1) {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i - 1][j] + matrix[i + 1][j] + matrix[i][j - 1]) / 4;
                        }
                        else {
                            matrixResult[i][j] = (matrix[i][j] + matrix[i - 1][j] + matrix[i + 1][j] + matrix[i][j - 1] + matrix[i][j + 1]) / 5;
                        }

                    }
                }

            }
        }
    }

    //clock_t end = clock();
    //double resultTime = (end - start) / (CLOCKS_PER_SEC / 1000);

    auto endTime = chrono::steady_clock::now();
    auto resultTime = chrono::duration_cast<chrono::milliseconds>(endTime - beginTime).count();
    cout << "Matrix m * n : " << m  << " * " << n << "\nThreads " << THREADS << "\nTime : " << resultTime  << endl;

    /*cout << "-------Result matrix-------" << endl;
    //print the matrix
    for (int a = 0; a < m; a++) {
        for (int b = 0; b < n; b++) {
            cout << matrixResult[a][b] << " ";
        }
        cout << endl;
    }*/


    delete[] matrix;
    delete[] matrixResult;

    return 0;
}
