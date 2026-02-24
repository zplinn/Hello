#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sha2-obf.h"

int main()
{
    sha2_lib_init();

    char *line = NULL;
    size_t linesize = 0;
    if (getline(&line, &linesize, stdin) == -1)
    {
        free(line);
        return 1;
    }
    line[strcspn(line, "\n")] = 0;
    uint8_t digest[SHA256_DIGEST_SIZE];

    linesize = strlen(line);
    sha256_obf(line, linesize, digest);
    for (size_t i = 0; i < SHA256_DIGEST_SIZE; i++)
    {
        printf("%02x", digest[i]);
    }
    printf("\n");

    return 0;
}