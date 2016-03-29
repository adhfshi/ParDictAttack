#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <mpi.h>
int main (int argc, char *argv[]) {
	FILE * f;
	std::string mpipassword;
	std::vector<std::string> mpipwarray;
	std::string password;
	std::vector<std::string> pwarray;
	int rank, size;
	double start_time, end_time, exe_time;
	long long int dict_size = 0;
	int num;

	if(argc < 2){
		std::cerr << "need load dictionary!! \n";
		return 1;
	}
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	if(rank==0){

		std::ifstream dict(argv[1]);

		int count=0;
		if(!dict){
			std::cerr << "No such file!! \n";
			return 1;
		}
		while(std::getline(dict, password)){
			pwarray.push_back(password);
			dict_size++;
		}
		dict.close();
		std::cout<<dict_size<<std::endl;
	}
	MPI_Bcast(&dict_size,1,MPI_LONG_LONG_INT,0,MPI_COMM_WORLD);
	num = dict_size%size;
	for(int count=0; count<(dict_size-num);count=count+size){
		if(rank == 0){
			mpipwarray.push_back(pwarray.at(count));
			int tempcount = count+1;
			for(int i=1;i<size;i++){
				char *tempstr = new char[pwarray.at(tempcount).length()];
				strcpy(tempstr,pwarray.at(tempcount).c_str());
				MPI_Send(tempstr,strlen(tempstr),MPI_CHAR,i,1,MPI_COMM_WORLD);
				tempcount++;
			}
		}else{
			MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
			int l;
			MPI_Get_count(&status, MPI_CHAR, &l);
			char *buf = new char[l];
			MPI_Recv(buf, l, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
			std::string pwstr(buf, l);
			mpipwarray.push_back(pwstr);
			
		//	delete [] buf;
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}

	if(num>0){
			int count = dict_size - num;
			if(rank == 0){
			mpipwarray.push_back(pwarray.at(count));
			int tempcount = count+1; 
			for(int i=1;i<num;i++){
				char *tempstr = new char[pwarray.at(tempcount).length()];
				strcpy(tempstr,pwarray.at(tempcount).c_str());
				MPI_Send(tempstr,strlen(tempstr),MPI_CHAR,i,1,MPI_COMM_WORLD);
			}
			}else if(rank < num){
				MPI_Probe(0, 1, MPI_COMM_WORLD, &status);
				int l;
				MPI_Get_count(&status, MPI_CHAR, &l);
				char *buf = new char[l];
				MPI_Recv(buf, l, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &status);
				std::string pwstr(buf, l);
				mpipwarray.push_back(pwstr);
			//	delete [] buf;	
			}
		}

	MPI_Barrier(MPI_COMM_WORLD);
	end_time = MPI_Wtime();
	exe_time = end_time - start_time;
	if(rank==0){
		printf( "Execution time is : %fs\n", exe_time);
	}
	std::cout<<mpipwarray.size()<<std::endl;
	MPI_Barrier(MPI_COMM_WORLD);
	std::cout<<rank<<"  "<<mpipwarray.back()<<std::endl;
	MPI_Finalize();
	return 0;
}

