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

class JacobiMPI{
public:
    JacobiMPI(int argc, char **argv);
    ~JacobiMPI();
    double run();

private:

    enum{
        DOUBLE_SEND_COEF = sizeof(double) / sizeof(MPI_DOUBLE), // it fails with ld
        INT_SEND_COEF    = sizeof(int) / sizeof(MPI_INT),
        MAIN_PROCESS     = 0,
    };


    ld** matrix;
    ld* result;
    ld* new_result;
    ld* free;
    double precision;

    std::string output;
    std::ofstream log;
    std::ofstream timeLog;
    int argc;
    char **argv;

    int processPartEnd;
    int processPartStart;
    int matrix_part;

    int matrix_rows;
    int matrix_cols;
    bool failed = false;
    double calc_time = -1;
    int MPI_size;
    int MPI_rank;


    ld getMaxVectorsDiff();
    bool solvePart(int index_from, int index_to);
    void startSolve();
    void printInfo();
    void prepareMPI(int argc, char **argv);
    void stopMPI();
    void mainProcessRun();
    void otherProcessRun();
    void writeResult();
    void readData();
    void broadcastInitial();
    void mergeResult();
    void initOthers();
};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
