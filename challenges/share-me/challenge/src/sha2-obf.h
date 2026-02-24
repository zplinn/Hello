#include <stdint.h>
#include <stddef.h>

#define SHA256_BLOCK_SIZE (512 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)

void sha2_lib_init();
void sha256_obf(const uint8_t *message, uint64_t len, uint8_t *digest);
