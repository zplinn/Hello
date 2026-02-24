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

#include <capstone/capstone.h>

char* gets(char*);

void win(void)
{
    system("/bin/sh");
}

void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

static jmp_buf g_continuation;

void segv_handler(int)
{
    longjmp(g_continuation, 1);
}

void fancy_sacrificial_routine(char* result, size_t length)
{
    int page_size = getpagesize();
    void* ptr = mmap(0, page_size * 2, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, 0, 0);
    if (ptr == MAP_FAILED)
    {
        perror("mmap");
        return;
    }
    if (mprotect(ptr + page_size, page_size, PROT_NONE) != 0)
    {
        perror("mprotect");
        munmap(ptr, page_size * 2);
        return;
    }

    printf("Behold: A memory-safe use of gets(), somehow.\n");
    printf("Surely nothing bad will happen to this extremely carefully crafted invocation!\n");
    printf("Enter text: ");

    if (setjmp(g_continuation))
    {
        printf("Well it SEGFAULTed but we caught it! Let's keep going and see what happens anyway.\n");
        goto gots;
    }

    // Convenient use of a stack pointer getting put into rax
    // *right* before the call to gets :D
    memset(result, 0, length);

    char* str_ptr = (char*)ptr;
    // nop this
    memset(str_ptr, 0, page_size);
    // nop this
    void (*original_segv_handler)(int) = signal(SIGSEGV, &segv_handler);
    
    // And then this clobbers whatever it gets
    gets(str_ptr);
    
gots:
    str_ptr[length - 1] = 0;
    strncpy(result, str_ptr, length);

    signal(SIGSEGV, original_segv_handler);
    munmap(ptr, page_size * 2);
}

int main(int argc, char** argv, char** envp)
{
    init();

    printf("Glitch Simulator 2000\n");
    
    void* region = (void*)((uintptr_t)((void*)&init) & ~0xfff);
    
    char* target = (char*)(uintptr_t)(void*)&fancy_sacrificial_routine;
    uintptr_t base = (uintptr_t)(void*)&fancy_sacrificial_routine;
    size_t length = ((uintptr_t)(void*)&main) - ((uintptr_t)(void*)&fancy_sacrificial_routine);
    mprotect(region, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC);
    csh handle;
    cs_insn *insn;
    size_t count;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return -1;
    count = cs_disasm(handle, (const uint8_t *)target, length, base, 0, &insn);
    if (count > 0)
    {
        for (size_t i = 0; i < count; i++)
        {
            printf("%#llx: %s %s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
        }

        printf("Glitch Simulator 2000\n");

        for (size_t n = 0; n < 8; n ++)
        {
            printf("Pick an address to nop (%zd/8): ", n + 1);
        
            uint64_t index = 0;
            scanf("%" SCNi64, &index);
            int _ = getchar();

            if (index < base || index >= (base + length))
            {
                printf("Out of range!\n");
                return 1;
            }
        
            for (size_t i = 0; i < count; i++)
            {
                if (insn[i].address == index)
                {
                    for (size_t j = 0; j < insn[i].size; j ++)
                    {
                        target[insn[i].address - base + j] = 0x90;
                    }
                }
            }
        }

        cs_free(insn, count);
    }
    else
    {
        printf("Failed to disassemble!\n");
        return -1;
    }
    cs_close(&handle);

    mprotect(region, 0x1000, PROT_READ | PROT_EXEC);

    printf("Now, let's do something sufficiently dangerous\n");
    char name[0x100];
    fancy_sacrificial_routine(&name[0], 0x100);
    printf("You entered: %s\n", name);

    return 0;
}
