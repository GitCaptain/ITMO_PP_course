#!/bin/bash
DEBUG=0

if [ $DEBUG == 1 ]; then
  path=cmake-build-debug-mpich
  flags=''
  echo -n debug
else
  path=cmake-build-release-mpich
  flags=-O3
  echo -n release
fi

path1=in
path2=inI
out_name_template=out
input_path=data
output_path=outputs
bin_name=parallel_programming_2

rm ${output_path}/log*
rm ${output_path}/out*

binary=${path}/${bin_name}
echo " name ${binary}"
mpicxx main.cpp jacobiMPI.cpp $flags -o $binary

rm ${output_path}/timelog
touch ${output_path}/timelog

for input in {0..2}
do
  echo input $input
  for processes in 1 2 4
  do
    echo "-n $processes $binary ${input_path}/${path1}${input} ${input_path}/${path2}${input} 0.001 ${output_path}/${out_name_template}${input}"
    mpiexec -n $processes $binary ${input_path}/${path1}${input} ${input_path}/${path2}${input} 0.001 ${output_path}/${out_name_template}${input} 2>>outputs/timelog
  done
done
