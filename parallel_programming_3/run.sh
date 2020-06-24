#!/binary/bash
DEBUG=1

if [ $DEBUG == 1 ]; then
  path=cmake-build-debug-mpich
  flags=''
  echo debug
else
  path=cmake-build-release-mpich
  flags=-O3
  echo release
fi

input=0
in_name_template=vec
out_name_template=out
input_path=inputs
output_path=outputs
bin_name=parallel_programming_3

rm ${output_path}/log*
rm ${output_path}/out*

binary=${path}/${bin_name}
mpicxx main.cpp quickSortMPI.cpp $flags -o $binary
mpiexec -n 4 $binary ${input_path}/${in_name_template}${input} ${output_path}/${out_name_template}${input}

#> outputs/timelog
#for input in {0..2}
#do
#  echo input $input
#  for processes in 1 2 4
#  do
#    echo -- with np $processes
#    mpiexec -n $processes ./${path}/parallel_programming_2 inputs/in${input} inputs/inI${input} 0.001 outputs/out${input} 2>>outputs/timelog
#  done
#done
