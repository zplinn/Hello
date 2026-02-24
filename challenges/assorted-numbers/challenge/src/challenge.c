#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>

void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    printf("SUDDEN DEATH !! ASSORTED NUMBERS !!\n");

    unsigned int foo[] = {
        53,
 10,
 50,
 34,
 22,
 54,
 46,
 5,
 16,
 7,
 41,
 45,
 6,
 25,
 32,
 43,
 2,
 33,
 61,
 26,
 9,
 55,
 59,
 14,
 20,
 52,
 17,
 28,
 12,
 48,
 47,
 62,
 35,
 19,
 38,
 63,
 51,
 39,
 27,
 18,
 23,
 42,
 21,
 44,
 4,
 37,
 31,
 0,
 58,
 3,
 36,
 13,
 30,
 8,
 1,
 24,
 11,
 57,
 56,
 15,
 49,
 29,
 40,
 60
    };

    for (int i = 0; i < 64; i ++)
    {
        printf("Which one is %d?\n", i);
        
        int which;
        scanf("%d", &which);

        if (which < 0 || which >= 64)
        {
            printf("That is not in range\n");
            return 1;
        }
        
        if (foo[which] != i)
        {
            printf("No, that one is %d\n", foo[which]);
            return 1;
        }
    }

    printf("You got them all!\n");
    system("/bin/sh");

    // printf("answer: %llx\n", something);

    return 1;
}
