#!/bin/bash

path_prefix=$(pwd)

binary=${path_prefix}/parallel_programming_2
mpicxx main.cpp jacobiMPI.cpp -O3 -o $binary

path1=${path_prefix}/in1
path2=${path_prefix}/inI1
output_path=${path_prefix}/out1
eps=0.001
processes=1

mpiexec -n ${processes} ${binary} ${path1} ${path2} ${eps} ${output_path}
