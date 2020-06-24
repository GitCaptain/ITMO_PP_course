#!/bin/bash
DEBUG=0

if [ $DEBUG == 1 ]; then
  path=cmake-build-debug-mpich
  flags=''
  echo debug
else
  path=cmake-build-release-mpich
  flags=-O3
  echo release
fi

input_num=0
in_name_template=vec
out_name_template=out
input_path=inputs
output_path=outputs
bin_name=parallel_programming_3
timelog=timelog

rm ${output_path}/log*
rm ${output_path}/out*
rm ${output_path}/${timelog}
touch ${output_path}/${timelog}

binary=${path}/${bin_name}
#mpicxx main.cpp quickSortMPI.cpp $flags -o $binary

for input_num in {0..6}; do
  echo calc ${input_num}
  in_file=${input_path}/${in_name_template}${input_num}
  out_file=${output_path}/${out_name_template}${input_num}
  pyout=${output_path}/py${input_num}
  python3 py/sort.py ${in_file} ${pyout}
  for processes in 1 2 4; do
    echo -- with np $processes
    mpiexec -n $processes ./${binary} ${in_file} ${out_file} 2>>${output_path}/${timelog}
    echo diff:
    diff ${pyout} ${out_file}
  done
done
