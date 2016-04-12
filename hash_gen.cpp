#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

void hashgen(unsigned char text[], unsigned char hash[]){	
	SHA256_CTX ctx;// = new SHA256_CTX;
	
	sha256_hash(&ctx, text, hash, 10000);
	//sha256_init(&ctx);
	//sha256_update(&ctx,text,strlen((const char*)text));
	//sha256_final(&ctx,hash);
	
}

int main(){
	unsigned char text1[]={"asczx8y2-sadh"},
				  text2[]={"21cw092km?"},
				  text3[]={"husky"},
				  text4[]={"EvcCxs90XZsa"},
				  text5[]={"Eis9Xi99asd"};
	unsigned char *hash = (unsigned char*)malloc(32*sizeof(unsigned char));

	FILE *fp = fopen("hash_value.dat1","w+");

	hashgen(text1,hash); //printf("%d \n", strlen((const char*)text3));
	fwrite(hash, sizeof(char), 32, fp);
	//memset((void*)&hash[0], 0, 32);
	hashgen(text2,hash);
	fwrite(hash, sizeof(char), 32, fp);
	//memset((void*)&hash[0], 0, 32);
	hashgen(text3,hash);
	fwrite(hash, sizeof(char), 32, fp);
	//memset((void*)&hash[0], 0, 32);
	hashgen(text4,hash);
	fwrite(hash, sizeof(char), 32, fp);
	//memset((void*)&hash[0], 0, 32);
	hashgen(text5,hash);
	fwrite(hash, sizeof(char), 32, fp);
	//memset((void*)&hash[0], 0, 32);
	fclose(fp);

}
