#include <iostream>
#include "jacobiMPI.h"

int main(int argc, char **argv) {

    JacobiMPI jmpi(argc, argv);
    double time = jmpi.run();

    if(time >= 0){
        std::cout << "calculation time: " << time << std::endl;
    }

    return 0;
}
