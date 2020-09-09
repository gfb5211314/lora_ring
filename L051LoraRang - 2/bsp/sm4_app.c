#include <string.h>
#include <stdio.h>
#include "sm4.h"
	unsigned char key[16] = {0x02,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
	unsigned char input1[16];
	unsigned char output1[16];
		sm4_context ctx;
	//数据加密
void data_encrypt(int length,unsigned char *input,
                   unsigned char *output)
{
	
    for(unsigned char  i=0;i<length;i++)
	
	{
		                    output[i]  =input[i]+1;
		
	}
//	sm4_setkey_enc(&ctx,key);
//	sm4_crypt_ecb(&ctx,1,length,input,output);
	 
}

void data_decrypt(int length,unsigned char *input,
                   unsigned char *output)
{
		
    for(unsigned char  i=0;i<length;i++)
	
	{
		                    output[i]  =input[i]-1;
		
	}
//	
//	sm4_setkey_dec(&ctx,key); 
//	sm4_crypt_ecb(&ctx,0,length,input,output);
	

}

//void test1_sm1()
//{
//	
//	
//}
int test_sm4()
{

//	sm4_context ctx;
	unsigned long i;

	//encrypt standard testing vector
	printf("加密\r\n");
data_encrypt(5,input1,output1);
	for(i=0;i<5;i++)
		printf("%02x ", output1[i]);
	printf("\n");
printf("解密\r\n");
	//decrypt testing
data_decrypt(5,output1,output1);
	for(i=0;i<5;i++)
		printf("%02x ", output1[i]);
	printf("\n");

//	//decrypt 1M times testing vector based on standards.
//	i = 0;
//	sm4_setkey_enc(&ctx,key);
//	while (i<1000000) 
//    {
//		sm4_crypt_ecb(&ctx,1,16,input,input);
//		i++;
//    }
//	for(i=0;i<16;i++)
//		printf("%02x ", input[i]);
//	printf("\n");
	
    return 0;
}
