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
        MAIN_PROCESS = 0,
        FREE,
        MATRIX,
    };


    ld** matrix;
    ld* result;
    ld* new_result;
    ld* free;
    ld precision;

    std::string output;
//    std::ofstream log;
    int argc;
    char **argv;

    int process_part_end;
    int process_part_start;
    int process_matrix_size;
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
    void sendInitial();
    void recvInitial();
    void mergeResult();
    void initOthers();
    std::pair<int, int> countProcessBounds(int process);
};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
