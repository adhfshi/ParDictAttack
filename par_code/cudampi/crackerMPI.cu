#include <stdio.h>
#include <fstream>
//#include <iostream>
#include <string.h>
//#include <vector>
#include <stdlib.h>
//#include <unistd.h>
//#include <time.h>

#include <cuda.h>
//#include <mpi.h>

#define uchar unsigned char // 8-bit byte
#define uint unsigned int // 32-bit word

//define for sha256
#define DBL_INT_ADD(a,b,c) if (a > 0xffffffff - (c)) ++b; a += c;
#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))

#define GPUerrchk(ans) { GPUassert((ans), __FILE__, __LINE__); }


typedef struct {
	uchar data[64];
	uint datalen;
	uint bitlen[2];
	uint state[8];
} SHA256_CTX;

__constant__ uint k[64] = {
   0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
   0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
   0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
   0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
   0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
   0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
   0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
   0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

__constant__ uchar answer[32];


//==============cuda kernel=====================================
__device__ void sha256_transform(SHA256_CTX *ctx, uchar *data){

   uint a,b,c,d,e,f,g,h,i,j,t1,t2,m[64];
      
   for (i=0,j=0; i < 16; ++i, j += 4)
      m[i] = (data[j] << 24) | (data[j+1] << 16) | (data[j+2] << 8) | (data[j+3]);
   for ( ; i < 64; ++i)
      m[i] = SIG1(m[i-2]) + m[i-7] + SIG0(m[i-15]) + m[i-16];

   a = ctx->state[0];
   b = ctx->state[1];
   c = ctx->state[2];
   d = ctx->state[3];
   e = ctx->state[4];
   f = ctx->state[5];
   g = ctx->state[6];
   h = ctx->state[7];
   
   for (i = 0; i < 64; ++i) {
      t1 = h + EP1(e) + CH(e,f,g) + k[i] + m[i];
      t2 = EP0(a) + MAJ(a,b,c);
      h = g;
      g = f;
      f = e;
      e = d + t1;
      d = c;
      c = b;
      b = a;
      a = t1 + t2;
   }
   
   ctx->state[0] += a;
   ctx->state[1] += b;
   ctx->state[2] += c;
   ctx->state[3] += d;
   ctx->state[4] += e;
   ctx->state[5] += f;
   ctx->state[6] += g;
   ctx->state[7] += h;

}

__device__ void sha256(SHA256_CTX *ctx, uchar *data, uchar *hash, int len){
		//init sha256 data structure
		ctx->datalen = 0;
   		ctx->bitlen[0] = 0;
   		ctx->bitlen[1] = 0;
   		ctx->state[0] = 0x6a09e667;
   		ctx->state[1] = 0xbb67ae85;
	   	ctx->state[2] = 0x3c6ef372;
   		ctx->state[3] = 0xa54ff53a;
	   	ctx->state[4] = 0x510e527f;
	   	ctx->state[5] = 0x9b05688c;
	   	ctx->state[6] = 0x1f83d9ab;
	   	ctx->state[7] = 0x5be0cd19;

		//update
		uint i;
		//uint len = 5; //need to fix!!
   		for (i=0; i < len; ++i) { 
    	  	ctx->data[ctx->datalen] = data[i]; 
      		ctx->datalen++; 
      		if (ctx->datalen == 64) { 
         		sha256_transform(ctx,ctx->data);
         		DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],512); 
         		ctx->datalen = 0;
 
      		}  
   		}

		//final
		i = ctx->datalen;

	   	if (ctx->datalen < 56) { 
    	  	ctx->data[i++] = 0x80; 
      		while (i < 56) 
         		ctx->data[i++] = 0x00; 
   		}	
   		else { 
      		ctx->data[i++] = 0x80; 
      		while (i < 64) 
         		ctx->data[i++] = 0x00; 
      	sha256_transform(ctx,ctx->data);
      	memset(ctx->data,0,56); 
   		}
		
		//par here
   		DBL_INT_ADD(ctx->bitlen[0],ctx->bitlen[1],ctx->datalen * 8);
   		ctx->data[63] = ctx->bitlen[0]; 
   		ctx->data[62] = ctx->bitlen[0] >> 8; 
   		ctx->data[61] = ctx->bitlen[0] >> 16; 
   		ctx->data[60] = ctx->bitlen[0] >> 24; 
   		ctx->data[59] = ctx->bitlen[1]; 
   		ctx->data[58] = ctx->bitlen[1] >> 8; 
   		ctx->data[57] = ctx->bitlen[1] >> 16;  
   		ctx->data[56] = ctx->bitlen[1] >> 24; 
   		sha256_transform(ctx,ctx->data);	

		//we can paralized at here
   		for (i=0; i < 4; ++i) { 
      		hash[i]    = (ctx->state[0] >> (24-i*8)) & 0x000000ff; 
      		hash[i+4]  = (ctx->state[1] >> (24-i*8)) & 0x000000ff; 
      		hash[i+8]  = (ctx->state[2] >> (24-i*8)) & 0x000000ff;
      		hash[i+12] = (ctx->state[3] >> (24-i*8)) & 0x000000ff;
      		hash[i+16] = (ctx->state[4] >> (24-i*8)) & 0x000000ff;
      		hash[i+20] = (ctx->state[5] >> (24-i*8)) & 0x000000ff;
      		hash[i+24] = (ctx->state[6] >> (24-i*8)) & 0x000000ff;
      		hash[i+28] = (ctx->state[7] >> (24-i*8)) & 0x000000ff;
   		}	

}

__device__ void sha256_hash(SHA256_CTX *ctx, uchar *data, uchar *hash, int len, int round){
	sha256(ctx, data, hash, len);
	while(round > 1){
		sha256(ctx,hash,hash,32);
		round --;
	}

}


__device__ bool my_strcmp(uchar *str_a, uchar *str_b, uint len){
	for(int i=0; i<len; i++){
		if(str_a[i] != str_b[i])
			return false;
	}
	
	return true;
}


__global__ void sha256_wrap(uchar *pwarray, uchar *target, int* pwcount, uchar *result){
	int idx = threadIdx.x + blockDim.x * blockIdx.x;
	uchar* data = (uchar*)malloc(pwcount[idx]*sizeof(uchar));

	SHA256_CTX ctx;// = new SHA256_CTX;	
	uchar hash[32];
	int round = 10000, count = 0;	

	for(int i=0; i<idx; i++){
		count += pwcount[i];
	}
	memcpy(data,&pwarray[count],pwcount[idx]*sizeof(uchar));

	sha256_hash(&ctx,data,hash,pwcount[idx],round);

	for (int i=0; i<5;i++){
		if(my_strcmp(hash,&target[32*i],32)){
			memcpy(result,data,pwcount[0]*sizeof(uchar));
		}	
	}
}


//====================================================================
void print_hash(unsigned char hash[]){

   int idx;
   for (idx=0; idx < 32; idx++)
      printf("%02x",hash[idx]);
   printf("\n");
}

extern "C" void hash_pairing(uchar *pwarray, uchar *target, int *pwcount, int num){

	uchar *dev_pwarray, *dev_target, *dev_result;
	uchar *result = new uchar[32]; 
	int *dev_pwcount;

	cudaMalloc((void**)&dev_pwarray,strlen((const char*)pwarray));
	cudaMemcpy((void*)dev_pwarray, pwarray, strlen((const char*)pwarray),cudaMemcpyHostToDevice);

	cudaMalloc((void**)&dev_target,160*sizeof(uchar));
	cudaMemcpy((void*)dev_target, target, 160*sizeof(uchar),cudaMemcpyHostToDevice);

	cudaMalloc((void**)&dev_result, 32*sizeof(uchar));
//	cudaMemcpy((void*)dev_result, result, 32*sizeof(uchar), cudaMemcpyHostToDevice);

	cudaMalloc((void**)&dev_pwcount,num*sizeof(int));
	cudaMemcpy((void*)dev_pwcount,pwcount,num*sizeof(int),cudaMemcpyHostToDevice);


	dim3 DimBlock(1024,1);
	dim3 DimGrid(55,1);
	sha256_wrap <<< DimGrid, DimBlock >>> (dev_pwarray, dev_target, dev_pwcount, dev_result);
	cudaDeviceSynchronize();
	
	cudaMemcpy((void*)result, dev_result, 32*sizeof(uchar), cudaMemcpyDeviceToHost);
	if(strlen((const char*)result)!=0)
		printf("password: %s ", result);

	memset(result,0,strlen((const char*) result));
	cudaDeviceReset();
	//cudaFree(dev_pwarray); cudaFree(dev_target); cudaFree(dev_result);
}

