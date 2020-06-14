//
// Created by alexander on 6/13/20.
//

#pragma once
#include <mpi.h>
#include <fstream>

using namespace std;

const int PRIMARY_PROCESS = 0;

class LinearSystemSolver {
private:
    int currentProcess;
    int totalProcesses;

    // this process will calculate a part [offset; offset + size] of array x
    int offset;
    int size;

    int systemSize;
    double precision;
    double *A;        // systemSize x systemSize; element[i][j] = A[i * systemSize + j]
    double *b;        // systemSize
    double *x;        // systemSize
    double *xPrevious = nullptr;   // systemSize

public:
    LinearSystemSolver() {
        MPI_Comm_rank(MPI_COMM_WORLD, &currentProcess);
        MPI_Comm_size(MPI_COMM_WORLD, &totalProcesses);
    }

    void initialize() {
        if (currentProcess == PRIMARY_PROCESS) {
            readParameters();
        }

        MPI_Bcast(&systemSize, 1, MPI_INT, PRIMARY_PROCESS, MPI_COMM_WORLD);
        MPI_Bcast(&precision, 1, MPI_DOUBLE, PRIMARY_PROCESS, MPI_COMM_WORLD);

        log("System size = " + to_string(systemSize) + ", precision = " + to_string(precision));

        if (currentProcess != PRIMARY_PROCESS) {
            A = new double[systemSize * systemSize];
            b = new double[systemSize];
            x = new double[systemSize];
        }

        MPI_Bcast(A, systemSize * systemSize, MPI_DOUBLE, PRIMARY_PROCESS, MPI_COMM_WORLD);
        MPI_Bcast(b, systemSize, MPI_DOUBLE, PRIMARY_PROCESS, MPI_COMM_WORLD);
        MPI_Bcast(x, systemSize, MPI_DOUBLE, PRIMARY_PROCESS, MPI_COMM_WORLD);

        log("A = " + toString(A, systemSize * systemSize) +
            ", b = " + toString(b, systemSize) +
            ", x = " + toString(x, systemSize));

        if (currentProcess == PRIMARY_PROCESS) {
            const int partSize = systemSize / totalProcesses;
            for (int process = 0; process < totalProcesses; process++) {
                int offset = process * partSize;
                int size = (process < totalProcesses - 1) ? partSize : systemSize - offset;
                if (process == PRIMARY_PROCESS) {
                    this->offset = offset;
                    this->size = size;
                } else {
                    MPI_Send(&offset, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
                    MPI_Send(&size, 1, MPI_INT, process, 0, MPI_COMM_WORLD);
                }
            }
        } else {
            MPI_Status status;
            MPI_Recv(&offset, 1, MPI_INT, PRIMARY_PROCESS, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Recv(&size, 1, MPI_INT, PRIMARY_PROCESS, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        }

        log("Offset = " + to_string(offset) + ", size = " + to_string(size));
    }

    void computeIteration() {
        saveCurrentXAsPrevious();
        computeProcessPart();
        exchangeArrayPartToWholeArray();
    }

    bool precisionReached() {
        if (xPrevious == nullptr) {
            return false;
        }
        for (int i = 0; i < systemSize; i++) {
            if (abs(x[i] - xPrevious[i]) > precision) {
                return false;
            }
        }
        return true;
    }

    void outputResult() {
        if (currentProcess == PRIMARY_PROCESS) {
            ofstream stream("linear-system.output");
            for (auto i = 0; i < systemSize; i++) {
                stream << x[i] << " ";
            }
            stream.close();
        }
    }

private:
    void readParameters() {
        ifstream stream("linear-system.input");
        stream >> systemSize >> precision;
        x = new double[systemSize];
        for (auto i = 0; i < systemSize; i++) {
            stream >> x[i];
        }
        b = new double[systemSize];
        for (auto i = 0; i < systemSize; i++) {
            stream >> b[i];
        }
        A = new double[systemSize * systemSize];
        for (auto i = 0; i < systemSize; i++) {
            for (auto j = 0; j < systemSize; j++) {
                stream >> A[i * systemSize + j];
            }
        }
        stream.close();
    }

    void saveCurrentXAsPrevious() {
        if (xPrevious == nullptr) {
            xPrevious = new double[systemSize];
        }
        for (int i = 0; i < systemSize; i++) {
            xPrevious[i] = x[i];
        }
    }

    void computeProcessPart() {
        for (int i = offset; i < offset + size; i++) {
            double sum = 0;
            for (int j = 0; j < systemSize; j++) {
                if (i != j) {
                    sum += A[i * systemSize + j] * xPrevious[j];
                }
            }
            x[i] = (b[i] - sum) / A[i * systemSize + i];
        }
    }

    void exchangeArrayPartToWholeArray() {
        MPI_Status status;
        const int partSize = systemSize / totalProcesses;
        for (int process = 0; process < totalProcesses; process++) {
            const int offset = process * partSize;
            const int size = (process < totalProcesses - 1) ? partSize : systemSize - offset;
            MPI_Bcast(&x[offset], size, MPI_DOUBLE, process, MPI_COMM_WORLD);
        }

        log("x = " + toString(x, systemSize));
    }

    void log(const string &message) {
        ofstream stream("ls." + to_string(currentProcess) + "-" + to_string(totalProcesses) + ".log", ios_base::app);
        stream << time(nullptr) << " - " << message << endl;
        stream.close();
    }

    static string toString(double *array, int size) {
        string contentString;
        if (size < 20) {
            for (int i = 0; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        } else {
            for (int i = 0; i < 5; i++) {
                contentString += to_string(array[i]) + " ";
            }
            contentString += " ... ";
            for (int i = size - 5; i < size; i++) {
                contentString += to_string(array[i]);
                if (i < size - 1) contentString += " ";
            }
        }
        return "|" + to_string(size) + "|[" + contentString + "]";
    }
};