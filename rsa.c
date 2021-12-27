/**
	@file		rsa.c
	@author		작성자 이름 / 학번
	@date		작성 일자
	@brief		mini RSA implementation code
	@details	세부 설명
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"

uint p, q, e, d, n;
uint result = 0;
/**
	@brief		모듈러 덧셈 연산을 하는 함수.
	@param		uint a		: 피연산자1.
	@param		uint b		: 피연산자2.
	@param		byte op		: +, - 연산자.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 덧셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/

uint Mod(uint a, uint mod){
	if(a > mod){  while(a >= mod) a -= mod; result = a;}
	else {result = a;}
	return result;
}

uint ModAdd(uint a, uint b, uint mod) { 

	if( a+b <= a) { //overflow
	 	result = Mod((a-(mod-b)) ,  mod);  
	}
	else {
		result = Mod( (a+b), mod);
	}
	return result;
}

/**
	@brief		모듈러 곱셈 연산을 하는 함수.
	@param		uint x		: 피연산자1.
	@param		uint y		: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 곱셈에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
*/
uint ModMul(uint x, uint y, uint mod) {

	uint a = 0 ;
	uint b = Mod(x, mod);
	while (y > 0){
		if(y & 1 == 1){
			a = ModAdd(a,b,mod);
		}
	b =  ModAdd(b,b,mod); 
	y = y >> 1;
	}
	result = Mod(a,mod); 	
	return result;
}

/**
	@brief		모듈러 거듭제곱 연산을 하는 함수.
	@param		uint base	: 피연산자1.
	@param		uint exp	: 피연산자2.
	@param		uint mod	: 모듈러 값.
	@return		uint result	: 피연산자의 연산에 대한 모듈러 연산 값.
	@todo		모듈러 값과 오버플로우 상황을 고려하여 작성한다.
				'square and multiply' 알고리즘을 사용하여 작성한다.
*/
uint ModPow(uint base, uint exp, uint mod) {
	uint x = 1;
	uint y = base;
	while (exp > 0){
		if (exp & 1 == 1){
			x =  ModMul(x,y,mod);
		}
		y = ModMul(y,y,mod);
		exp = exp >> 1 ;
	}
	result =  x;
	return result;
}

/**
	@brief		입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
	@param		uint testNum	: 임의 생성된 홀수.
	@param		uint repeat		: 판단함수의 반복횟수.
	@return		uint result		: 판단 결과에 따른 TRUE, FALSE 값.
	@todo		Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
				이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다. 
*/
bool IsPrime(uint p , uint repeat) {
    if (p < 2) {return 0;}
    if (p != 2 && (p & 1)==0){return 0;}   // 2가 아니고 짝수라면 제외
    uint s = p - 1;
	
    while ( (s & 1) == 0){s = s >> 1;}
	
    for (int i = 0; i < repeat ; i++){
        uint a ,temp = s;
		
		
	while(1){a = (WELLRNG512a() * 100000);
	if( 1< a && a < p){break;}
	}
        uint mod = ModPow(a, temp, p);
        while (temp != p - 1 && mod != 1 && mod != p - 1)
        {
            mod = ModMul(mod, mod, p);
            temp *= 2;
        }
        if (mod != p - 1 && (temp & 1) == 0)
        {
            return 0;
        }
    }
    return 1;
}

uint GCD(uint a, uint b) {
	uint prev_a;

	while(b != 0) {
		printf("GCD(%u, %u)\n", a, b);
		prev_a = a;
		a = b;
		while(prev_a >= b) prev_a -= b;
		b = prev_a;
	}
	printf("GCD(%u, %u)\n\n", a, b);
	return a;
}

/**
	@brief		모듈러 역 값을 계산하는 함수.
	@param		uint a	: 피연산자1.
	@param		uint m	: 모듈러 값.
	@return		uint result	: 피연산자의 모듈러 역수 값.
	@todo		확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
*/
uint ModInv(uint a, uint m) {
	uint result, u1, u3, v1, v3, t1, t3, q;
	long iter;
	u1 = 1; u3 = a; v1 = 0;v3 = m; 
	iter = 1;
	while (v3 != 0) {
		q = u3 / v3; t3 = Mod(u3,v3);   
		t1 = u1 + q * v1;  u1 = v1;
		v1 = t1; u3 = v3;v3 = t3;
		iter *= -1;
	}
	if (u3 != 1)
		return 0;
	if (iter < 0)
		result = m - u1;
	else
		result = u1;
	return result;
}

/**
	@brief		RSA 키를 생성하는 함수.
	@param		uint *p	: 소수 p.
	@param		uint *q	: 소수 q.
	@param		uint *e	: 공개키 값.
	@param		uint *d	: 개인키 값.
	@param		uint *n	: 모듈러 n 값.
	@return		void
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
void MRSAKeygen(uint *p, uint *q, uint *e, uint *d, uint *n) {

	printf("mRSA key generator start.\n ");
	uint state[16]; uint temp = 0; uint qn;
	while(1){
		temp = (WELLRNG512a() * 100000);  
		if(32768 < temp && temp < 65536){ 
			printf("random_number1 %u selected \n",temp);
			if (IsPrime(temp, 4)){
				printf("%u may be Prime\n",temp);
				*q = temp; break;	//temp			
			}printf("%u is not Prime \n",temp);
		}
	}
	while(1){
		temp = (WELLRNG512a() * 100000);
		if(32768 < temp && temp < 65536){ 
			printf("random_number2 %u selected \n",temp);
			if (IsPrime(temp,4)){
				printf("%u may be Prime\n",temp);
				*p = temp; break;				
			}printf("%u is not Prime\n",temp);
		 }
	}
	printf("finally selected prim q : %u  p : %u \n" ,*q , *p);
	*n = (*p) * (*q); qn = (*p -1 ) * (*q -1 );
	printf("thus, n = %u  qn =  %u \n" , *n ,qn);
	while(1){
		*e = (WELLRNG512a() * 10000000000);
		if(  1 < *e && *e < qn){printf(" e = %u selected\n",*e);
			if ( GCD(*e,qn) == 1){break;}
		}
	}
	*d = ModInv(*e ,qn);
	printf(" d = %u selected\n",*d);
	printf(" e , d , n , qn : %u - %u - %u - %u \n",*e,*d,*n,qn);
	printf(" e*d mod qn : %u \n",ModMul(*e,*d,qn));
}

/**
	@brief		RSA 암복호화를 진행하는 함수.
	@param		FILE *ifp	: 입력 파일 포인터.
	@param		uint len	: 입력 파일 길이.
	@param		FILE *ofp	: 출력 파일 포인터.
	@param		uint key	: 키 값.
	@param		uint n		: 모듈러 n 값.
	@return		uint result	: 암복호화에 성공한 byte 수.
	@todo		과제 안내 문서의 제한사항을 참고하여 작성한다.
*/
uint MRSACipher(FILE *ifp, uint len, FILE *ofp, uint key, uint n) {
	printf("MRSACipher start. file len is %u \n",len);
	uint buf[len] ; 
	uint buf2[len]; //buffer 
	fread(buf,4,(len/4),ifp);
	for(int i = 0 ; i < (len/4) ; i++)
	{		
		printf("len : %d \nptx : %u \nctx : %u \n"
			,(i+1) *4, buf[i] , ModPow(buf[i],key,n) );
		buf2[i] = ModPow(buf[i],key,n);
	}	
	fwrite(buf2,4,(len/4),ofp);
	result = len;
	return result;
}

void main(int argc, char const *argv[]) {
	uint seed = time(NULL);
	InitWELLRNG512a(&seed);

	FILE *data_fp, *enc_fp, *dec_fp ,*padding_fp;
	uint fsize;

	if(argc != 4) {
		printf("usage : ./rsa data_file encrypt_file decrypt_file\n");
		exit(1);
	}
	data_fp = fopen(argv[1], "rb");
	padding_fp = fopen(argv[1], "a");
	fwrite("  ",2,1,padding_fp);
	enc_fp = fopen(argv[2], "wb");
	if(data_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}
	
	fseek(data_fp, 0, SEEK_END);
	fsize = ftell(padding_fp);
	printf("data file size : %u\n\n", fsize);   
	fseek(data_fp, 0, SEEK_SET);

	
	MRSAKeygen(&p, &q, &e, &d, &n);
	fsize = MRSACipher(data_fp, fsize, enc_fp, e, n);

	fclose(data_fp);
	fclose(enc_fp);

	enc_fp = fopen(argv[2], "rb");
	dec_fp = fopen(argv[3], "wb");
	if(dec_fp == NULL | enc_fp == NULL) {
		printf("file open fail\n");
		exit(1);
	}

	printf("encrypted file size : %u\n\n", fsize);

	fsize = MRSACipher(enc_fp, fsize, dec_fp, d, n);



	fclose(enc_fp);
	fclose(dec_fp);
}
