#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#include <cuda.h>
#include <string.h>
//#include <fstream>
//#include <iostream>

//using namespace std;

extern void hash_pairing(unsigned char *pwarray, unsigned char *target, int* pwcount, int num);


void print_hash(unsigned char hash[]){

   int idx;
   for (idx=0; idx < 32; idx++)
      printf("%02x",hash[idx]);
   printf("\n");
}

void read_hash(char filename[], unsigned char *target){
	FILE *rhash = fopen(filename,"r+");
	fread(target,1,160,rhash);
	//fread(&target[32],1,32,rhash);
	//fread(&target[64],1,32,rhash);
	//fread(&target[96],1,32,rhash);
	//fread(&target[128],1,32,rhash);
    fclose(rhash);
	for(int i =0; i<5; i++)
		print_hash(&target[32*i]);
}




void main(int argc, char **argv){
	FILE *f;
	long long int line = 0;
	unsigned char *mpipassword;
	unsigned char buff[32];
	int *pwcount;

	int rank, size;
	
	unsigned char target[160]; 

	if(argc<2){
		printf("input arguments invalid \n");
		return ;
	}
	
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	if(rank==0){
		read_hash(argv[2],target);
			
		unsigned char tbuff[32];
		FILE *tf;
		tf = fopen(argv[1],"r+");
		while(fgets(tbuff,32,(FILE*)tf) != NULL){
			line++;
		}
		fclose(tf);
	}
	MPI_Bcast(&line,1,MPI_LONG_LONG_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(target,160, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);

	mpipassword = (unsigned char*)malloc((line/size+1)*32*sizeof(unsigned char));
	pwcount = (int*)malloc((line/size+1)*sizeof(int));
	
	if(rank == 0){
		int i = 0; int tempcount = 0;
		f = fopen(argv[1],"r+");
		while(fgets(buff,32,(FILE*)f)!=NULL){
			if(i % size == 0){
				pwcount[i/size] = strlen(buff)-1;
                //printf("pwcount: %.6s \n", &mpipassword[tempcount]);
				memcpy(&mpipassword[tempcount], buff, pwcount[i/size]);
                tempcount += pwcount[i/size];
			}else{
                int rec = i % size;
                MPI_Send(buff,strlen(buff)-1,MPI_CHAR,rec,1,MPI_COMM_WORLD);
            }
            i++;
		}
		fclose(f);
	}
	else{
		int tempcount = 0;
		for(int i = 0; i < line/size; i++){
			int len;
			MPI_Probe(0,1,MPI_COMM_WORLD, &status);
			MPI_Get_count(&status,MPI_CHAR, &len);
			MPI_Recv(buff, len, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			pwcount[i] = len;
			memcpy(&mpipassword[tempcount], buff, len);
			tempcount += len;
		}
//		printf("mpipw: %.10s \n", mpipassword);
	}
	MPI_Barrier(MPI_COMM_WORLD);	

	hash_pairing(mpipassword, target, pwcount, line/size+1);
	MPI_Finalize();

}
