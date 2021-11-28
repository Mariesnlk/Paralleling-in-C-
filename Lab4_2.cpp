#include <time.h>
#include <omp.h>

#include <cstring>
#include <vector>
#include <thread>
#include <iostream>
using namespace std;

int getRandomNumber(int min, int max) {
	int num = min + rand() % (max - min + 1);
	return num;
}

// поразрядная сортировка с использованием OpenMP
void radixSort_omp(int* data, int n, int nThreads) {
	int* temp = new int[n];
	unsigned char* byteP = (unsigned char*)data;
	int byte = 0;

	int* counter = new int[256];
	int* counters = new int[nThreads * 256];

	for (int j = 0; j < sizeof(int); j++, byte++) {
		memset(counter, 0, 256 * sizeof(int));
		memset(counters, 0, 256 * sizeof(int) * nThreads);
		int* threadCounter;

#pragma omp parallel firstprivate(threadCounter, byteP)
		{
			int tid = omp_get_thread_num();
			threadCounter = counters + 256 * tid;

			// каждый поток получает на обработку часть массива
			// и выполняет подсчет элементов в свой массив подсчетов threadCounter.
#pragma omp for
			for (int i = 0; i < n; i++) {
				byteP = (unsigned char*)data + i * sizeof(int) + byte;
				threadCounter[*byteP]++;
			}
		}

		// c помощью массивов подсчетов со всех потоков выполняется вычисление смещений,
		// по которым будут располагаться элементы при втором проходе по массиву.
		for (int k = 0; k < nThreads; k++) {
			threadCounter = counters + 256 * k;
			for (int i = 0; i < 256; i++) {
				counter[i] += threadCounter[i];
			}
		}

		for (int i = 1; i < 256; i++) {
			counter[i] += counter[i - 1];
		}

		for (int i = nThreads - 1; i >= 0; i--) {
			threadCounter = counters + 256 * i;
			for (int k = 0; k < 256; k++) {
				counter[k] -= threadCounter[k];
				threadCounter[k] = counter[k];
			}
		}

#pragma omp parallel firstprivate(threadCounter, byteP)
		{
			int tid = omp_get_thread_num();
			threadCounter = counters + 256 * tid;

			// каждый поток получает на обработку ту же часть массива, что и ранее,
			// и выполняет копирование элемента во вспомогательный массив
			// по соответствующему индексу в массиве смещений.
#pragma omp for
			for (int i = 0; i < n; i++) {
				byteP = (unsigned char*)data + i * sizeof(int) + byte;
				temp[threadCounter[*byteP]++] = data[i];
			}
		}

		swap(data, temp);
	}

	delete[] temp;
	delete[] counter;
	delete[] counters;
}


int main(int argc, char* argv[]) {

	int n;
	int nThreads;
	printf("Enter the size of the array : ");
	scanf_s("%d", &n);
	printf("Enter the size of the threads : ");
	scanf_s("%d", &nThreads);
	int* data1 = new int[n];
	int* data2 = new int[n];

	double t1, t2;

	for (int i = 0; i < n; i++) {
		data1[i] = getRandomNumber(1, 100);
		data2[i] = data1[i];
	}

	omp_set_num_threads(nThreads);
	t1 = omp_get_wtime();
	auto beginTime = chrono::steady_clock::now();
	//	radixSort_omp(data1, n, nThreads);


	int* temp = new int[n];
	unsigned char* byteP = (unsigned char*)data1;
	int byte = 0;

	int* counter = new int[256];
	int* counters = new int[nThreads * 256];

	for (int j = 0; j < sizeof(int); j++, byte++) {
		memset(counter, 0, 256 * sizeof(int));
		memset(counters, 0, 256 * sizeof(int) * nThreads);
		int* threadCounter;

#pragma omp parallel firstprivate(threadCounter, byteP)
		{
			int tid = omp_get_thread_num();
			threadCounter = counters + 256 * tid;

			// каждый поток получает на обработку часть массива
			// и выполняет подсчет элементов в свой массив подсчетов threadCounter.
#pragma omp for
			for (int i = 0; i < n; i++) {
				byteP = (unsigned char*)data1 + i * sizeof(int) + byte;
				threadCounter[*byteP]++;
			}
		}

		// c помощью массивов подсчетов со всех потоков выполняется вычисление смещений,
		// по которым будут располагаться элементы при втором проходе по массиву.
		for (int k = 0; k < nThreads; k++) {
			threadCounter = counters + 256 * k;
			for (int i = 0; i < 256; i++) {
				counter[i] += threadCounter[i];
			}
		}

		for (int i = 1; i < 256; i++) {
			counter[i] += counter[i - 1];
		}

		for (int i = nThreads - 1; i >= 0; i--) {
			threadCounter = counters + 256 * i;
			for (int k = 0; k < 256; k++) {
				counter[k] -= threadCounter[k];
				threadCounter[k] = counter[k];
			}
		}

#pragma omp parallel firstprivate(threadCounter, byteP)
		{
			int tid = omp_get_thread_num();
			threadCounter = counters + 256 * tid;

			// каждый поток получает на обработку ту же часть массива, что и ранее,
			// и выполняет копирование элемента во вспомогательный массив
			// по соответствующему индексу в массиве смещений.
#pragma omp for
			for (int i = 0; i < n; i++) {
				byteP = (unsigned char*)data1 + i * sizeof(int) + byte;
				temp[threadCounter[*byteP]++] = data1[i];
			}
		}

		swap(data1, temp);
	}

	delete[] temp;
	delete[] counter;
	delete[] counters;

	t2 = omp_get_wtime();
	cout << "N : " << n << " Threads : " << nThreads << " Time : " << t2 - t1 << endl;



	delete[] data1;
	delete[] data2;

	return 0;
}
