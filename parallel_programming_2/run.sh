#!/bin/bash
DEBUG=0

if [ $DEBUG == 1 ]
then
  path=cmake-build-debug-mpich
  echo debug
else
  path=cmake-build-release-mpich
  echo release
fi

mpicxx main.cpp jacobiMPI.cpp -O3 -o $path/parallel_programming_2
> outputs/timelog
for input in {0..2}
do
  echo input $input
  for processes in 1 2 4
  do
    echo -- with np $processes
    mpiexec -n $processes ./${path}/parallel_programming_2 inputs/in${input} inputs/inI${input} 0.001 outputs/out${input} 2>>outputs/timelog
  done
done

