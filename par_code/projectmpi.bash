#!/bin/sh
#BSUB -J Zhiyou-project-mpi
#BSUB -o projectmpiout
#BSUB -e projectmpierr
#BSUB -n 20
#BUSB -R "span[ptile=5]"
#BSUB -q ht-10g
#BSUB cwd /home/wang.zhiy/project
work=/home/wang.zhiy/project

cd $work
tempfile1=hostlistrun
tempfile2=hostlist-tcp
echo $LSB_MCPU_HOSTS > $tempfile1
declare -a hosts
read -a hosts < ${tempfile1}
for ((i=0; i<${#hosts[@]}; i += 2)) ;
  do
   HOST=${hosts[$i]}
   CORE=${hosts[(($i+1))]}
   echo $HOST:$CORE >> $tempfile2
done
#####################################################
#####
###DO NOT EDIT ANYTHING ABOVE THIS LINE#########
#####################################################

rm -f projectmpiout
rm -f projectmpierr
mpirun -np 20 -prot -TCP -lsf ./projectmpi2 10_million_password_list_top_10000.txt
