#include <iostream>
#include "matrix.h"




int main(int argc, char **argv) {

    if(argc < 5){
        std::cout << "usage: path1 path2 double path3\n"
                     "path1 - path to matrix\n"
                     "path2 - path to initial approximation\n"
                     "double - precision value\n"
                     "path3 - path for output.\n";
    }



    return 0;
}
