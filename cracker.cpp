#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "sha256.h"


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
	unsigned char target[5][32];
	unsigned char hash[32];
	unsigned char *pw;
	SHA256_CTX ctx;
	clock_t start,end;

	if(argc < 2){
		std::cerr << "need load dictionary!! \n";
		return 1;
	}
	//timing program
	start = clock();
	read_hash(argv[2],target);
	//f = fopen(argv[1], "r");
	std::ifstream dict(argv[1]);
	if(!dict){
		std::cerr << "No such file!! \n";
		return 1;
	}

	while(std::getline(dict, password)){
		//calculate password hash
		pw = (unsigned char *)password.c_str();
		sha256_init(&ctx);
		sha256_update(&ctx, pw, strlen((const char*)pw));
		sha256_final(&ctx, hash);
		for(int i=0; i<5; i++){
			if(compare_hash(hash, target[i])){
				std::cout<< "password found: " << password << std::endl;
				break;
			}
		}
	}
	end = clock();
	printf("\n\ncpu time is: %f \n",((double)(end-start))/CLOCKS_PER_SEC);

	return 0;
}
