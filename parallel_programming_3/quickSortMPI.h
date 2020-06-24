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
        MAIN_PROCESS = 0,
    };

    enum tags{
        ARRAY,
        SIZE,
        INIT,
        PIVOT
    };

    int full_array_size = 0; // only for main process
    int* full_array = nullptr; // only for main process
    int* array_working_part = nullptr;
    int array_size = 0;
    int previous_array_size = 0;
    double calc_time = -1;
    int MPI_initial_size = -1;
    int MPI_initial_rank = -1;
    int iteration = 0;

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
    void waitInitialData();
    void sendResult();
    void quickSort(int from, int to);
    void sendInitialData();
    int getPivot(int from, int to);
};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
