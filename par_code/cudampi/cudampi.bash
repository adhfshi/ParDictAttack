#!/bin/sh
#BSUB -J sample1
#BSUB -o output_file
#BSUB -e error_file
#BSUB -n 5
#BSUB -q par-gpu-2
#BSUB -R "span[ptile=5]"
#BSUB -cwd /home/shi.hao/hpc_proj/cudampi/

work=/home/shi.hao/hpc_proj/cudampi/

cd $work
rm -f error_file output_file

mpirun -np 5 -prot -TCP -lsf ./final sample.txt hash_value.dat
