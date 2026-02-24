#include <stdio.h>
#include <string.h>

#include "obfuscate.h"
#include "check.h"

int main()
{
    printf("Please input flag: ");

    char flag[256];
    if(scanf("%255s", flag) != 1) {
        printf("Failed to read input\n");
        return 1;
    }
    size_t flaglen = strcspn(flag, "\n");
    flag[flaglen] = 0;
    if(check((uint8_t*)flag, flaglen) == 1) {
        printf("Correct! Congratulations!\n");
        return 0;
    } else {
        printf("Incorrect! Try again!\n");
        return 1;
    }
}

#include "check.c"
