#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <setjmp.h>
#include <dlfcn.h>
#include <sys/prctl.h>

const unsigned char payload[] = {
#include "payload.h"
};
const size_t payload_len = sizeof(payload) / sizeof(unsigned char);

void init(void)
{
    setvbuf(stdin, NULL, _IOFBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

    printf("SUDDEN DEATH! Shellcode decryptor!\n");

    printf("I have already written the shellcode for you!\n");
    printf("What should I xor the shellcode with? (1 byte hex)\n");

    uint8_t key;
    scanf("%" SCNx8, &key);

    unsigned char* mem = (unsigned char*)mmap(0, payload_len, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
    for (int i = 0; i < payload_len; i ++)
    {
        mem[i] = payload[i] ^ key;
    }
    mprotect(mem, payload_len, PROT_READ | PROT_EXEC);

    ((void(*)())mem)();
    
    return 0;
}
