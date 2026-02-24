#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <pwd.h>

#include "stage2.h"
#include "sha2-obf.h"
#include "rc4.h"

// #define DEBUG 1

#define USERNAME_LEN 5

extern uint8_t __start_text2;
extern uint8_t __stop_text2;

int main()
{
    sha2_lib_init();

    struct passwd *user = getpwuid(getuid());

    if (user == NULL)
    {
        return 1;
    }

#ifdef DEBUG
    char *username = "Pr1m3";
#else
    char *username = user->pw_name;
#endif

    if (strlen(username) < USERNAME_LEN)
    {
        return 1;
    }

    char username_lower[USERNAME_LEN];
    for (size_t i = 0; i < USERNAME_LEN; i++)
    {
        username_lower[i] = tolower(username[i]);
    }

    uint8_t digest[SHA256_DIGEST_SIZE];
    sha256_obf(username_lower, USERNAME_LEN, digest);

    uint8_t *start_aligned = (uint8_t *)(((uint64_t)(uint8_t *)&__start_text2) & ~0xfff);
    size_t text2_len = &__stop_text2 - start_aligned;
    if (mprotect(start_aligned, text2_len, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
    {
        return 1;
    }

#ifdef DEBUG
    printf("Key = ");
    for (size_t i = 0; i < SHA256_DIGEST_SIZE; i++)
    {
        printf("%02x", digest[i]);
    }
    printf("\n");
#endif

    RC4(digest, SHA256_DIGEST_SIZE, (uint8_t *)&__start_text2, (uint8_t *)&__start_text2, &__stop_text2 - &__start_text2);
    if (mprotect(start_aligned, text2_len, PROT_READ | PROT_EXEC) != 0)
    {
        return 1;
    }

    int correct = 0;
    for (size_t i = 0; i < (&__stop_text2 - &__start_text2); i++)
    {
        uint64_t *checkptr = (uint64_t *)(((uint8_t *)&__start_text2) + i);
        if (*checkptr == 0x133713381339133A)
        {
            correct = 1;
            break;
        }
    }
    if (correct == 0)
    {
        return 1;
    }

    if (stage2_validate() != 0x133713381339133A)
    {
        return 1;
    }

    if (stage2() == 1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
