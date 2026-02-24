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

    printf("SUDDEN DEATH !! YOU CAN JUST BYPASS THE TRICKY MATH !!\n");
    printf("(or emulate it or something, it's not *that* tricky)\n");

    uint64_t something = 12345;
    uint64_t something_else = 0x12345;
    for (int i = 0; i < 100; i ++)
    {
        switch (i % 7) {
        case 0:
        case 2:
        case 6:
            something ^= (something << 3) + 0x195;
            break;
        case 1:
            something_else ^= (something << 9) + 0x7294;
        case 3:
        case 4:
            something_else ^= (something_else << 7) + 0x965127;
            break;
        case 5:
            something ^= something_else;
            break;
        }
    }


    printf("What did it compute?\n");

    uint64_t guess;
    scanf("%llx", &guess);

    if (guess == something)
    {
        printf("Yes, that's it!\n");
        system("/bin/sh");
        return 0;
    }

    // printf("answer: %llx\n", something);

    return 1;
}
