#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include "data.h"
#include "rc4.h"

#define KEYLEN 16

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int64_t calculate(int8_t *data, size_t len, size_t *start, size_t *end) {
    size_t best_start = 0;
    size_t best_end = 0;
    int64_t best_val = 0;
    for(size_t cand_start = 0; cand_start < len; cand_start++) {
        for(size_t cand_end = cand_start + 1; cand_end < len; cand_end++) {
            int64_t cand_val = 0;
            for(size_t pos = cand_start; pos < cand_end; pos++) {
                cand_val += data[pos];
            }

            if(cand_val > best_val) {
                best_val = cand_val;
                best_start = cand_start;
                best_end = cand_end;
            }
        }
    }

    *start = best_start;
    *end = best_end;
    return best_val;
}

int main(int argc, char** argv, char** envp)
{
    init();

    size_t best_start = 0;
    size_t best_end = 0;
    int64_t best_value = calculate(data, data_len, &best_start, &best_end);

    //printf("Best: %ld, start: %ld, end: %ld\n", best_value, best_start, best_end);

    uint8_t key[KEYLEN];
    for(size_t i = 0; i < KEYLEN; i++) {
        key[i] = 0; 
    }

    size_t answer_len = best_end - best_start;
    for(size_t i = 0; i < answer_len; i++) {
        key[i % KEYLEN] = (key[i % KEYLEN] + data[best_start + i]) & 0xFF;
    }

    /*printf("Key = ");
    for(size_t i = 0; i < KEYLEN; i++) {
        printf("%02hhx", key[i]);
    }
    printf("\n");*/

    RC4(key, KEYLEN, flag_enc, flag_enc, flag_enc_len);

    printf("Flag: %s\n", flag_enc);

    return 0;
}
