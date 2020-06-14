//
// Created by alexander on 6/13/20.
//

#include <mpi.h>
#include "newsolve.cpp"

int main() {
    MPI_Init(nullptr, nullptr);

    LinearSystemSolver solver;
    solver.initialize();
    for (int iteration = 0; iteration < 1000 && !solver.precisionReached(); iteration++) {
        solver.computeIteration();
    }
    solver.outputResult();

    MPI_Finalize();
    return solver.precisionReached() ? 0 : -1;
}