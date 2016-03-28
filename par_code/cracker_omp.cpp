#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <time.h>
#include <omp.h>
#include "sha256.h"

//================================================================================


bool compare_hash(unsigned char password[], unsigned char tgt_hash[]){
	
	if(memcmp(password,tgt_hash,32) == 0)
		return true;
	else 
		return false;

}


void print_hash(unsigned char hash[]){

   int idx;
   for (idx=0; idx < 32; idx++)
      printf("%02x",hash[idx]);
   printf("\n");
}

void read_hash(char filename[],unsigned char target[5][32]){
	FILE *rhash = fopen(filename,"r+");
	fread(target[0],1,32,rhash);
	fread(target[1],1,32,rhash);
	fread(target[2],1,32,rhash);
	fread(target[3],1,32,rhash);
	fread(target[4],1,32,rhash);
	fclose(rhash);
}


int main(int argc, char **argv){
	FILE * f;
	std::string password;
	std::vector<std::string> pwarray;
	unsigned char target[5][32];
	//unsigned char hash[32];
	//unsigned char *pw;
	//SHA256_CTX ctx;
	float start,end;
	unsigned long long int dict_size = 0;

	if(argc < 2){
		std::cerr << "need load dictionary!! \n";
		return 1;
	}
	//timing program
	start = omp_get_wtime();
	read_hash(argv[2],target);

	std::ifstream dict(argv[1]);
	if(!dict){
		std::cerr << "No such file!! \n";
		return 1;
	}
	while(std::getline(dict, password)){
		pwarray.push_back(password);
		dict_size++;
	}
	dict.close();
	
	#pragma omp parallel
	{
		#pragma omp for schedule(static)
		for(int i=0;i<dict_size;i++){
			//calculate password hash
			//std::getline(dict1, password);
			//std::cout<< pwarray.at(i) << std::endl;
			//std::cout<< i << std::endl;
			SHA256_CTX ctx;
			unsigned char hash[32];
			unsigned char *pw = (unsigned char *)pwarray.at(i).c_str();
			sha256_hash(&ctx,pw,hash,100000);

			for(int j=0; j<5; j++){
				if(compare_hash(hash, target[j])){
					std::cout<< "password found: " << pwarray.at(i) << std::endl;
					break;
				}
			}
		}
	}
	#pragma omp barrier	
	end = omp_get_wtime();
	printf("\n\ncpu time is: %f \n",end-start);

	return 0;
}
