#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

void hashgen(unsigned char text[], unsigned char hash[]){	
	SHA256_CTX ctx;
	
	sha256_hash(&ctx, text, hash,10000);
	//sha256_init(&ctx);
	//sha256_update(&ctx,text,strlen((const char*)text));
	//sha256_final(&ctx,hash);
	
}

int main(){
	unsigned char text1[]={"redsox29"},
				  text2[]={"zipcar22"},
				  text3[]={"husky"},
				  text4[]={"chicken92"},
				  text5[]={"Eis9Xi99asd"},
				  hash[32];

	FILE *fp = fopen("hash_value.dat1","w+");

	hashgen(text1,hash);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	memset((void*)&hash[0], 0, 32);
	hashgen(text2,hash);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	memset((void*)&hash[0], 0, 32);
	hashgen(text3,hash);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	memset((void*)&hash[0], 0, 32);
	hashgen(text4,hash);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	memset((void*)&hash[0], 0, 32);
	hashgen(text5,hash);
	fwrite(hash, sizeof(char), sizeof(hash), fp);
	memset((void*)&hash[0], 0, 32);
	fclose(fp);

}
