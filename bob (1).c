/**************************
 *      Homework 6        *
 ************************** 
 *
 *Compile:          gcc alice.c -lssl -lcrypto -o alice
 *                  gcc bob.c -lssl -lcrypto -o bob
 * 
 *Run:              ./alice Message.txt Seed.txt
 *                  ./bob Message.txt PK_Hex.txt R_Hex s_Hex
 *
 *Documentation:    SSL Documentation: https://www.openssl.org/docs/manmaster/man3/
 *
 *   OpenSSL Doc on SHA256: https:  //www.openssl.org/docs/manmaster/man3/SHA256.html
 *   OpenSSL Doc on BN Context: https:   //www.openssl.org/docs/manmaster/man3/BN_CTX_new_ex.html
 *   OpenSSL Doc on BIGNUM conversions:  https://www.openssl.org/docs/manmaster/man3/BN_hex2bn.html
 *   OpenSSL Doc on EC_GROUP:  https://www.openssl.org/docs/manmaster/man3/EC_GROUP_get_point_conversion_form.html
 *   OpenSSL Doc on EC_POINT: https://www.openssl.org/docs/manmaster/man3/EC_POINT_bn2point.html
 *
 * Created By:      << Saleh Darzi >>
_______________________________________________________________________________*/

//Header Files
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/bn.h>



//Function Prototypes
unsigned char* Read_File (char fileName[], int *fileLen);
void Write_File(char fileName[], char input[] );
void Convert_to_Hex(char output[], unsigned char input[], int inputlength);
unsigned char* Hash_SHA256(unsigned char* input, unsigned long inputlen);
unsigned char* get_sha256_hash(const EC_GROUP *group, const char *message, size_t messLength, const EC_POINT *R, BN_CTX *bn_ctx);

/*************************************************************
						M A I N
**************************************************************/
int main (int argc, char* argv[])
{	
	/*********************************************
	* part (c) D-Schnorr Signature Verifications *
	**********************************************/
	//step1: read the message
	int message_length = 0;
	unsigned char *message = Read_File(argv[1], &message_length);
	
	//step2: read Alice’s public key
	BN_CTX *bn_ctx;
	bn_ctx = BN_CTX_new();
	
	EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp192k1);
	const EC_POINT *G = EC_GROUP_get0_generator(group);
	
	int s_hex_len = 0;
	char *ss = Read_File(argv[4], &s_hex_len);
	
	BIGNUM *s = BN_new();
	BN_hex2bn(&s, ss);
	
	EC_POINT *s_G = EC_POINT_new(group);
	EC_POINT_mul(group, s_G, NULL, G, s, bn_ctx);
	char *s_G_hex = EC_POINT_point2hex(group, s_G, POINT_CONVERSION_UNCOMPRESSED,
	bn_ctx);
	
	int alice_public_key_length = 0;
	char *key = Read_File(argv[2], &alice_public_key_length);
	
	//step3: read Alice’s signature
	int R_length = 0;
	char *r = Read_File(argv[3], &R_length);
	
	EC_POINT *Y = EC_POINT_new(group);
	EC_POINT_hex2point(group, key, Y, bn_ctx);
	
	EC_POINT *R = EC_POINT_new(group);
	EC_POINT_hex2point(group, r, R, bn_ctx);
	
	unsigned char* s_hash_hex = get_sha256_hash(group, message, message_length, R,
	bn_ctx);
	
	//step4:computes the right part of the verification formula
	
	BIGNUM *s_hash_bn = BN_new();
	BN_hex2bn(&s_hash_bn, s_hash_hex);
	EC_POINT *s_hash_Y = EC_POINT_new(group);
	EC_POINT_mul(group, s_hash_Y, NULL, Y, s_hash_bn, bn_ctx);
	//step5: Verify the signature correctness
	char *s_hash = EC_POINT_point2hex(group, s_hash_Y, POINT_CONVERSION_UNCOMPRESSED,
	bn_ctx);
	EC_POINT *r_calculated = EC_POINT_new(group);
	EC_POINT_add(group, r_calculated, s_G, s_hash_Y, bn_ctx);
	char *Real = EC_POINT_point2hex(group, r_calculated,
	POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
	int v = EC_POINT_cmp(group, R, r_calculated, bn_ctx);
	
	//step 6
	if(v == 0){
		Write_File("Verification_Result.txt", "Successful Verification on Bob Side");
	}
	else{
		Write_File("Verification_Result.txt", "Verification Failed on Bob Side");
	}
	

    	return 0;
}
/*************************************************************
					F u n c t i o n s
**************************************************************/

/*******************************
        BIGNUM Functions
********************************/
/*================================
    Creating Context for BIGNUM
==================================*/
//BN_CTX *bn_ctx;
//bn_ctx = BN_CTX_new();

/*============================
    Convert BIGNUM to HEX
==============================*/
char *BN_bn2hex(const BIGNUM *a);

/*============================
    Convert HEX to BIGNUM
==============================*/
int BN_hex2bn(BIGNUM **a, const char *str);

/*==================================
    BIGNUM modular multiplication   r = a*b mod m
====================================*/
//int BN_mod_mul(BIGNUM *r, BIGNUM *a, BIGNUM *b, const BIGNUM *m, BN_CTX *ctx);

/*===============================
    BIGNUM modular subtraction  r =  a-b mod m
=================================*/
//int BN_mod_sub(BIGNUM *r, BIGNUM *a, BIGNUM *b, const BIGNUM *m, BN_CTX *ctx);

//==============================================================================================================================

/***************************************
      EC_POINT and EC_GROUP Functions
****************************************/
/*==============================================
    Creating EC_POINT and Freeing it at the end
================================================*/
EC_POINT *EC_POINT_new(const EC_GROUP *group);
void EC_POINT_free(EC_POINT *point);

/*==============================================
    Getting the conversion form from group
================================================*/
point_conversion_form_t EC_GROUP_get_point_conversion_form(const EC_GROUP *group);

/*============================
    Convert EC_POINT to HEX
==============================*/
char *EC_POINT_point2hex(const EC_GROUP *group, const EC_POINT *p, point_conversion_form_t form, BN_CTX *ctx);

/*============================
    Convert HEX to EC point
==============================*/
EC_POINT *EC_POINT_hex2point(const EC_GROUP *group, const char *hex, EC_POINT *p, BN_CTX *ctx);

/*=======================================
    EC Point to usigned char conversion
=========================================*/
size_t EC_POINT_point2buf(const EC_GROUP *group, const EC_POINT *point, point_conversion_form_t form, unsigned char **pbuf, BN_CTX *ctx);

/*============================
    EC Point Multiplication
==============================*/
/*--- Description:
*   EC_POINT_mul calculates the value generator * n + q * m and stores the result in r. 
*   The value n may be NULL in which case the result is just q * m (variable point multiplication). 
*   Alternatively, both q and m may be NULL, and n non-NULL, in which case the result is 
*   just generator * n (fixed point multiplication). When performing a single fixed or 
*   variable point multiplication, the underlying implementation uses a constant time algorithm, 
*   when the input scalar (either n or m) is in the range [0, ec_group_order).
*/
int EC_POINT_mul(const EC_GROUP *group, EC_POINT *r, const BIGNUM *n, const EC_POINT *q, const BIGNUM *m, BN_CTX *ctx);

/*============================
    EC Point Comparison
==============================*/
int EC_POINT_cmp(const EC_GROUP *group, const EC_POINT *a, const EC_POINT *b, BN_CTX *ctx);

//==============================================================================================================================
/*============================
    EC Group creationg
==============================*/
/*--- Description:
*   Use "NID_secp192k1" as the nid
*/
EC_GROUP *EC_GROUP_new_by_curve_name(int nid);

/*===================================
    EC Group point conversion form
=====================================*/
point_conversion_form_t EC_GROUP_get_point_conversion_form(const EC_GROUP *group);

/*============================
    EC Group generator
==============================*/
const EC_POINT *EC_GROUP_get0_generator(const EC_GROUP *group);

/*============================
    EC Group order "q"
==============================*/
int EC_GROUP_get_order(const EC_GROUP *group, BIGNUM *order, BN_CTX *ctx);

/*============================
    SHA256 Hash Function
==============================*/
/*--- Description:
*   SHA256() computes the SHA-256 message digest of the "count" bytes at data and places it in md.
*   SHA256() returns a pointer to the hash value.
*/
unsigned char *SHA256(const unsigned char *data, size_t count, unsigned char *md_buf);


/*============================
        Read from File
==============================*/
unsigned char* Read_File (char fileName[], int *fileLen)
{
    FILE *pFile;
	pFile = fopen(fileName, "r");
	if (pFile == NULL)
	{
		printf("Error opening file.\n");
		exit(0);
	}
    fseek(pFile, 0L, SEEK_END);
    int temp_size = ftell(pFile)+1;
    fseek(pFile, 0L, SEEK_SET);
    unsigned char *output = (unsigned char*) malloc(temp_size);
	fgets(output, temp_size, pFile);
	fclose(pFile);

    *fileLen = temp_size-1;
	return output;
}

/*============================
        Write to File
==============================*/
void Write_File(char fileName[], char input[]){
  FILE *pFile;
  pFile = fopen(fileName,"w");
  if (pFile == NULL){
    printf("Error opening file. \n");
    exit(0);
  }
  fputs(input, pFile);
  fclose(pFile);
}
/*============================
        Showing in Hex 
==============================*/
void Show_in_Hex(char name[], unsigned char hex[], int hexlen)
{
	printf("%s: ", name);
	for (int i = 0 ; i < hexlen ; i++)
   		printf("%02x", hex[i]);
	printf("\n");
}

/*============================
        Convert to Hex 
==============================*/
void Convert_to_Hex(char output[], unsigned char input[], int inputlength)
{
    for (int i=0; i<inputlength; i++){
        sprintf(&output[2*i], "%02x", input[i]);
    }
    //printf("Hex format: %s\n", output);  //remove later
}

unsigned char* Hash_SHA256(unsigned char* input, unsigned long inputlen) {
    unsigned char *hash_result = (unsigned char*)malloc(SHA256_DIGEST_LENGTH);
    
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, input, inputlen);
    SHA256_Final(hash_result, &sha256_ctx);
    
    return hash_result;
}

unsigned char* get_sha256_hash(const EC_GROUP *group, const char *message, size_t messLength, const EC_POINT *R, BN_CTX *bn_ctx) {
    unsigned char *R_buf = NULL;
    int R_length = EC_POINT_point2buf(group, R, POINT_CONVERSION_UNCOMPRESSED, &R_buf, bn_ctx);

    // Allocate memory for se and initialize it with message
    unsigned char *se = (unsigned char*)calloc(messLength + R_length, sizeof(unsigned char));
    memcpy(se, message, messLength);
    memcpy(se + messLength, R_buf, R_length);

    unsigned char *sha_result = (unsigned char*)malloc(SHA256_DIGEST_LENGTH);
    
    SHA256_CTX sha256_ctx;
    SHA256_Init(&sha256_ctx);
    SHA256_Update(&sha256_ctx, se, messLength + R_length);
    SHA256_Final(sha_result, &sha256_ctx);

    // Convert the hash to hexadecimal
    unsigned char *sha_hex = (unsigned char*)malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    for (size_t i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf((char*)&sha_hex[i * 2], "%02x", sha_result[i]);
    }
    sha_hex[SHA256_DIGEST_LENGTH * 2] = '\0';

    free(R_buf);
    free(se);
    free(sha_result);
    return sha_hex;
}

//__________________________________________________________________________________________________________________________
