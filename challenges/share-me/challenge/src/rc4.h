#include <stddef.h>
#include <stdint.h>

int RC4(uint8_t *key, size_t keylen, uint8_t *plaintext, uint8_t *ciphertext, size_t len);
