#!/bin/sh
#BSUB -J ompcracker
#BSUB -o omp.o
#BSUB -e omp.err
#BSUB -n 1
#BSUB -q ht-10g
#BSUB -cwd /home/shi.hao/hpc_proj

work=/home/shi.hao/hpc_proj/

cd $work
rm omp.o omp.err
export OMP_NUM_THREADS=10
./crackeromp 10_million_password_list_top_10000.txt hash_value.dat

