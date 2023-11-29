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

/*************************************************************
						M A I N
**************************************************************/
int main (int argc, char* argv[])
{
	BN_CTX *bn_ctx;
	bn_ctx = BN_CTX_new();
	
	/************************************
	* part (a) D-Schnorr Key Generation *
	*************************************/
	
	//step1: reads the message
	int message_length = 0;
	unsigned char *message = Read_File(argv[1], &message_length);
	
	//step2: reads the seed
	int seed_length = 0;
	unsigned char *seed = Read_File(argv[2], &seed_length);
	
	//step3: hash the seed to obtain the Private key
	unsigned char *seed_hash= Hash_SHA256(seed, seed_length);
	char private_key[64];
	Convert_to_Hex(private_key, seed_hash, 32);
	Write_File("Seedhash.txt", private_key);
	
	//step4: uses the private key to obtain the public key 	
	EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp192k1);
	const EC_POINT *G = EC_GROUP_get0_generator(group);
	char * G_hex = EC_POINT_point2hex(group, G, POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
	
	BIGNUM * y_bn_key = BN_new();
	BN_hex2bn(&y_bn_key, private_key);
	EC_POINT * public_key = EC_POINT_new(group);
	EC_POINT_mul(group, public_key, y_bn_key, G, NULL, bn_ctx);
	//step5: write the Hex format of the key pairs in “SK Hex.txt” and “PK Hex.txt”,
	char *public_key_hex = EC_POINT_point2hex(group, public_key,
	POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
	Write_File("PK_Hex.txt", public_key_hex);
	Write_File("SK_Hex.txt", private_key);
	
	/******************************************
	* part (b) D-Schnorr Signature Generation *
	*******************************************/
	
	//step1: concatenates the message and the private key 
	
	unsigned char *r = malloc(65);
	memcpy(r, message, message_length);
	memcpy(r + message_length, seed_hash, 32);
	//r[64+1] = '\0';
	
	unsigned char *hash = Hash_SHA256(r, 64);
	char R_hex[64];
	Convert_to_Hex(R_hex, hash, 32);
	
	//step 2: Perform the scalar multiplication
	BIGNUM *r_bn = BN_new();
	BN_hex2bn(&r_bn, R_hex);
	
	EC_POINT *r_point = EC_POINT_new(group);
	EC_POINT_mul(group, r_point, r_bn, G, NULL, bn_ctx);
	
	//step3: obtain “s” part of the signature
	unsigned char *r_buf = NULL;
	int R_length = EC_POINT_point2buf(group, r_point, POINT_CONVERSION_UNCOMPRESSED, &r_buf,
	bn_ctx);
	
	unsigned char *s = malloc(message_length + R_length +1);
	memcpy(s, message, message_length);
	memcpy(s + message_length, r_buf, R_length);
	
	
	unsigned char* s_hash = Hash_SHA256(s, message_length + R_length);
	char s_hash_hex[64];
	Convert_to_Hex(s_hash_hex, s_hash, 32);
	
	// Converting SHA256(m||R) to BIGNUM
	BIGNUM *s_bn = BN_new();
	BN_hex2bn(&s_bn, s_hash_hex);
	
	// Converting y to BIGNUM
	BIGNUM *y_bn = BN_new();
	BN_hex2bn(&y_bn, private_key);
	
	// Getting q
	BIGNUM *q = BN_new();
	EC_GROUP_get_order(group, q, bn_ctx);
	
	// multiplication
	BIGNUM *value = BN_new();
	BN_mod_mul(value, s_bn, y_bn_key, q, bn_ctx);
	
	//subtraction
	BIGNUM *sub = BN_new();
	BN_mod_sub(sub, r_bn, value, q, bn_ctx);
	
	//step4: write the Hex format of the signature in “R Hex.txt” and “s Hex.txt”
	char *s_hex = BN_bn2hex(sub);
	char *r_hex = EC_POINT_point2hex(group, r_point, POINT_CONVERSION_UNCOMPRESSED,
	bn_ctx);
	
	Write_File("s_Hex.txt", s_hex);
	Write_File("R_Hex.txt", r_hex);
	
	free(r);
	free(s);
	free(s_hex);
	free(r_hex);
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
//__________________________________________________________________________________________________________________________
