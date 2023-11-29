#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <unistd.h>

/* Function to calculate SHA256 hash */
unsigned char* calculateSHA256(unsigned char* input, unsigned long inputlen) {
    unsigned char* hash_result = (unsigned char*) malloc(SHA256_DIGEST_LENGTH);
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, inputlen);
    SHA256_Final(hash_result, &sha256);
    return hash_result;
}

/* Function to read from file */
unsigned char* readFile(char fileName[], int *fileLen) {
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("Error opening file.\n");
        exit(0);
    }
    fseek(pFile, 0L, SEEK_END);
    int temp_size = ftell(pFile) + 1;
    fseek(pFile, 0L, SEEK_SET);
    unsigned char *output = (unsigned char*) malloc(temp_size);
    fgets(output, temp_size, pFile);
    fclose(pFile);
    *fileLen = temp_size - 1;
    return output;
}

/* Function to write to file */
void writeFile(char fileName[], char input[]) {
    FILE *pFile = fopen(fileName, "w");
    if (pFile == NULL) {
        printf("Error opening file. \n");
        exit(0);
    }
    fputs(input, pFile);
    fclose(pFile);
}

/* Function to convert byte array to hexadecimal string */
void convertToHex(char output[], unsigned char input[], int inputlength) {
    for (int i = 0; i < inputlength; i++) {
        sprintf(&output[2 * i], "%02x", input[i]);
    }
}

/* Function to perform SHA256 hash and return hexadecimal string */
unsigned char* getSHA256Hash(const EC_GROUP *group, const char *message, size_t messLength, const EC_POINT *R, BN_CTX *bn_ctx) {
    unsigned char *R_buf = NULL;
    int R_length = EC_POINT_point2buf(group, R, POINT_CONVERSION_UNCOMPRESSED, &R_buf, bn_ctx);
    
    unsigned char *se = (unsigned char*) calloc(messLength + R_length, sizeof(unsigned char));
    memcpy(se, message, messLength);
    memcpy(se + messLength, R_buf, R_length);

    unsigned char *sha = calculateSHA256(se, messLength + R_length);
    unsigned char *sha_hex = (unsigned char*) malloc(SHA256_DIGEST_LENGTH * 2 + 1);
    convertToHex(sha_hex, sha, SHA256_DIGEST_LENGTH);

    free(R_buf);
    free(se);
    return sha_hex;
}

int main (int argc, char* argv[]) {
    BN_CTX *bn_ctx = BN_CTX_new();
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp192k1);
    const EC_POINT *G = EC_GROUP_get0_generator(group);

    int s_len = 0;
    char *ses = readFile(argv[4], &s_len);
    BIGNUM *s = BN_new();
    BN_hex2bn(&s, ses);

    EC_POINT *sG = EC_POINT_new(group);
    EC_POINT_mul(group, sG, NULL, G, s, bn_ctx);

    int messLength = 0;
    unsigned char *message = readFile(argv[1], &messLength);

    int AlicePubKeylen = 0;
    char *key = readFile(argv[2], &AlicePubKeylen);

    int R_len = 0;
    char *Re = readFile(argv[3], &R_len);

    EC_POINT *Y = EC_POINT_new(group);
    EC_POINT_hex2point(group, key, Y, bn_ctx);

    EC_POINT *R = EC_POINT_new(group);
    EC_POINT_hex2point(group, Re, R, bn_ctx);

    unsigned char* sha_hex = getSHA256Hash(group, message, messLength, R, bn_ctx);

    BIGNUM *sha_bn = BN_new();
    BN_hex2bn(&sha_bn, sha_hex);

    EC_POINT *shaY = EC_POINT_new(group);
    EC_POINT_mul(group, shaY, NULL, Y, sha_bn, bn_ctx);

    EC_POINT *CalculatedR = EC_POINT_new(group);
    EC_POINT_add(group, CalculatedR, sG, shaY, bn_ctx);

    int valid = EC_POINT_cmp(group, R, CalculatedR, bn_ctx);
    if (valid == 0) {
        writeFile("Verification_Result.txt", "Successful Verification on Bob Side");
    } else {
        writeFile("Verification_Result.txt", "Verification Failed on Bob Side");
    }
    return 0;
}
