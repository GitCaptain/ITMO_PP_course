//
// Created by alexander on 6/11/20.
//

#ifndef PARALLEL_PROGRAMMING_2_JACOBIMPI_H
#define PARALLEL_PROGRAMMING_2_JACOBIMPI_H

#include <vector>
#include "matrix.h"
#include <mpi.h>


class JacobiMPI{
public:
    JacobiMPI(int argc, char **argv);
    ~JacobiMPI();
    double run();

private:

    enum MPI_tags{
        FREE,
        MATRIX,
        PRECISION,
        RESULT,
        INFO,
        FAIL,
        DONE,
    };

    Matrix matrix;
    std::vector<ld> result;
    std::vector<ld> free;
    double precision;
    bool failed = false;
    double calc_time = -1;
    const int MAIN_PROCESS = 0;

    int MPI_size;
    int MPI_rank;

    int argc;
    char **argv;

    ld getMaxVectorsDiff(const std::vector<ld> &a, const std::vector<ld> &b);
    void solve(int index_from, int index_to);
    void prepareMPI(int argc, char **argv);
    void stopMPI();
    void mainProcessRun();
    void otherProcessRun();

    std::vector<ld> getResult();
    void sendResult(int tag=RESULT);
};

#endif //PARALLEL_PROGRAMMING_2_JACOBIMPI_H
