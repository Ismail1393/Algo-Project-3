#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/ecdsa.h>
#include <openssl/sha.h>
#include <openssl/ec.h>
#include <openssl/bn.h>

/* Function to compute SHA256 hash */
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

int main(int argc, char* argv[]) {
    /* Check for proper arguments */

    // Initialize OpenSSL contexts
    BN_CTX *bn_ctx = BN_CTX_new();

    // Read message and seed from files
    int messageLength = 0, seedLength = 0;
    unsigned char *message = readFile(argv[1], &messageLength);
    unsigned char *seed = readFile(argv[2], &seedLength);

    // Compute SHA256 hash of the seed
    unsigned char* seedHash = calculateSHA256(seed, seedLength);
    
    // Convert seedHash to hexadecimal string
    char privateKeyHex[65];
    convertToHex(privateKeyHex, seedHash, SHA256_DIGEST_LENGTH);

    // Write seedHash to file
    writeFile("Seedhash.txt", privateKeyHex);

    // Initialize EC_GROUP using secp192k1 curve
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp192k1);
    const EC_POINT *G = EC_GROUP_get0_generator(group);

    // Convert privateKeyHex to BIGNUM
    BIGNUM *y_bnkey = BN_new();
    BN_hex2bn(&y_bnkey, privateKeyHex);

    // Compute Public Key
    EC_POINT *publicKey = EC_POINT_new(group);
    EC_POINT_mul(group, publicKey, y_bnkey, G, NULL, bn_ctx);

    // Convert Public Key to hexadecimal string
    char *publicKeyHex = EC_POINT_point2hex(group, publicKey, POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
    writeFile("PK_Hex.txt", publicKeyHex);
    writeFile("SK_Hex.txt", privateKeyHex);

    /* Part 2 */
    unsigned char *r = malloc(messageLength + SHA256_DIGEST_LENGTH + 1);
    memcpy(r, message, messageLength);
    memcpy(r + messageLength, seedHash, SHA256_DIGEST_LENGTH);
    r[messageLength + SHA256_DIGEST_LENGTH] = '\0';

    // Hash the concatenation result
    unsigned char *hash = calculateSHA256(r, messageLength + SHA256_DIGEST_LENGTH);

    // Convert hash to hexadecimal string
    char rHex[65];
    convertToHex(rHex, hash, SHA256_DIGEST_LENGTH);

    // Convert rHex to BIGNUM
    BIGNUM *r_bn = BN_new();
    BN_hex2bn(&r_bn, rHex);

    // Perform scalar multiplication r.G
    EC_POINT *R = EC_POINT_new(group);
    EC_POINT_mul(group, R, r_bn, G, NULL, bn_ctx);

    // Convert EC_POINT R to buffer
    unsigned char *R_buf = NULL;
    int R_len = EC_POINT_point2buf(group, R, POINT_CONVERSION_UNCOMPRESSED, &R_buf, bn_ctx);

    // Concatenate message and R
    unsigned char *se = malloc(messageLength + R_len);
    memcpy(se, message, messageLength);
    memcpy(se + messageLength, R_buf, R_len);

    // Hash the concatenated data
    unsigned char* sha = calculateSHA256(se, messageLength + R_len);

    // Convert SHA256(m||R) to hexadecimal string
    char shaHex[65];
    convertToHex(shaHex, sha, SHA256_DIGEST_LENGTH);

    // Convert shaHex to BIGNUM
    BIGNUM *s_bn = BN_new();
    BN_hex2bn(&s_bn, shaHex);

    // Convert y_bnkey to BIGNUM
    BIGNUM *y_bn = BN_new();
    BN_hex2bn(&y_bn, privateKeyHex);

    // Get EC_GROUP order q
    BIGNUM *q = BN_new();
    EC_GROUP_get_order(group, q, bn_ctx);

    // Compute s value
    BIGNUM *val = BN_new();
    BN_mod_mul(val, s_bn, y_bnkey, q, bn_ctx);
    BIGNUM *s = BN_new();
    BN_mod_sub(s, r_bn, val, q, bn_ctx);

    // Convert s to hexadecimal string
    char* sHex = BN_bn2hex(s);
    writeFile("s_Hex.txt", sHex);

    // Convert EC_POINT R to hexadecimal string
    char *RHex = EC_POINT_point2hex(group, R, POINT_CONVERSION_UNCOMPRESSED, bn_ctx);
    writeFile("R_Hex.txt", RHex);

    return 0;
}
