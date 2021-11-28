#include <iostream>
#include <mpi.h>
#include <cmath>

using namespace std;

// source\repos\MPI\x64\Release>mpiexec -n 3 ./MPI.exe

const int vectorSize = 1000000;
const int iterations = 100;

const int elem1 = 0;
const int elem2 = vectorSize / 2 - 1;
const int elem3 = vectorSize - 1;
const double fixedValue = 1;


int main(int argc, char** argv) {

    // create vectors
    double* vector = new double[vectorSize];
    for (int i = 0; i < vectorSize; ++i) {
        vector[i] = 0;
    }

    vector[elem1] = fixedValue;
    vector[elem2] = fixedValue;
    vector[elem3] = fixedValue;

    double* resultVector = new double[vectorSize];
    for (int i = 0; i < vectorSize; ++i) {
        resultVector[i] = 0;
    }
    resultVector[elem1] = fixedValue;
    resultVector[elem2] = fixedValue;
    resultVector[elem3] = fixedValue;

    // Initialize MPI
    MPI_Init(&argc, &argv);

    int procsNum, procsRank;
    // Get the number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &procsNum);
    // Get the rank of this process 
    MPI_Comm_rank(MPI_COMM_WORLD, &procsRank);

    MPI_Status status;


    int* procs = new int[procsNum];
    int N = vectorSize;

    //set the number of elements in each process
    for (int i = 0; i < procsNum - 1; i++) {
        procs[i] = ceil(N / double(procsNum - i));
        N -= procs[i];
    }
    procs[procsNum - 1] = N;

    // expose indexes from which to which we fill in certain process
    int* startIndexInVector = new int[procsNum];
    startIndexInVector[0] = 0;
    for (int i = 1; i < procsNum; ++i) {
        startIndexInVector[i] = startIndexInVector[i - 1] + procs[i - 1];
    }

    int to = procsRank - 1;
    int from = procsRank + 1;
    if (to == -1) {
        to = procsNum - 1;
    }
    if (from == procsNum) {
        from = 0;
    }

    //returns the current walltime
    double startTime = MPI_Wtime();

    for (int s = 0; s < iterations; ++s) {
        // split vector calculations into parts between processes and pass between them last items
        for (int i = startIndexInVector[procsRank]; i < startIndexInVector[procsRank] + procs[procsRank]; i++) {
            resultVector[i] = (vector[i - 1] + vector[i + 1]) / 2;
        }


        if (procsNum > 1) {
            //convey the beginning, which is the end
            MPI_Send(&resultVector[startIndexInVector[procsRank]], 1, MPI_DOUBLE, to, 0, MPI_COMM_WORLD);
            MPI_Recv(&resultVector[startIndexInVector[procsRank] + procs[procsRank]], 1, MPI_DOUBLE, from, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            //convey the end, which is the beginning
            MPI_Send(&resultVector[startIndexInVector[procsRank] + procs[procsRank] - 1], 1, MPI_DOUBLE, from, 0, MPI_COMM_WORLD);
            MPI_Recv(&resultVector[startIndexInVector[procsRank] - 1], 1, MPI_DOUBLE, to, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        resultVector[elem1] = fixedValue;
        resultVector[elem2] = fixedValue;
        resultVector[elem3] = fixedValue;

        double* temp = vector;
        vector = resultVector;
        resultVector = temp;
    }

    // send calculated parts to 0 process
    if (procsRank != 0) {
        MPI_Send(&vector[startIndexInVector[procsRank]], procs[procsRank], MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    // receive calculated parts from each process and collect together
    else {
        for (int i = 1; i < procsNum; ++i) {
            MPI_Recv(&vector[startIndexInVector[i]], procs[i], MPI_DOUBLE, i, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }
    }

    double endTime = MPI_Wtime();

    if (procsRank == 0) {
        // Results
        cout << "Vector size: " << vectorSize << "\n" <<
            "Iterations: " << iterations << "\n" <<
            "Threads: " << procsNum << "\n" <<
            "Time: " << endTime - startTime << " seconds" << endl;
       /* cout << "mainVector [";
        for (int i = 0; i < N; i++) {
            cout << vector[i] << ' ';
        }
        cout << "]";*/
    }

    // Finalize MPI
    MPI_Finalize();
}
