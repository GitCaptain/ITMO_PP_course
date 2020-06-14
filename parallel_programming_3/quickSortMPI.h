//
// Created by alexander on 6/11/20.
//

#ifndef PARALLEL_PROGRAMMING_2_JACOBIMPI_H
#define PARALLEL_PROGRAMMING_2_JACOBIMPI_H

#include <vector>
#include "mpi.h"
#include <string>
#include <fstream>

typedef long double ld;

class QuickSortMPI{
public:
    QuickSortMPI(int argc, char **argv);
    ~QuickSortMPI();
    double run();

private:

    enum consts{
        INT_SEND_COEF    = sizeof(int) / sizeof(MPI_INT),
        MAIN_PROCESS     = 0,
    };

    enum tags{
        ARRAY,
        SIZE,
    };

    int* array = nullptr;
    int array_size = 0;
    int previous_array_size = 0;
    double calc_time = -1;
    int MPI_size = -1;
    int MPI_rank = -1;
    int iteration = 0;
    int parent_process = -1;

    int argc;
    char **argv;

    std::vector<MPI_Request> requests;
    std::ofstream log;

    void readInitialData();
    void rearrangePart(int from, int to);
    void startSolve();
    void printInfo();
    void prepareMPI();
    void stopMPI();
    void mainProcessRun();
    void otherProcessRun();
    void writeResult();
    void spawnProcess(int child_process);
    void waitInitialData();
    void sendResult();
    void quickSort(int from, int to);
    void doOrder();
};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
