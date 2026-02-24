#include <stdio.h>
#include <stdlib.h>
#include <machine/syscall.h>

#define BUFFER_SIZE 4096

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void* mmap(void* ptr, unsigned long size, unsigned int prot, unsigned int flags, int fd, unsigned long offset)
{
	register void* a0 asm("a0") = ptr;
	register unsigned long a1 asm("a1") = size;
	register unsigned int a2 asm("a2") = prot;
	register unsigned int a3 asm("a3") = flags;
	register int a4 asm("a4") = fd;
	register unsigned long a5 asm("a5") = offset;
	register unsigned int a7 asm("a7") = SYS_mmap;
	__asm__ __volatile__("ecall\n"
		: "=r" (a0)
		: "r" (a0), "r" (a1), "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a7)
		: "memory"
	);
	return a0;
}

int main(int argc, char** argv, char** envp)
{
    init();

	char* buf = mmap(NULL, BUFFER_SIZE, 7, 0x22, -1, 0);

    puts("Give me some bytes and I promise I'll run them if you follow our policy.");
	puts("You must use minimal RISC-V instructions. Compressed instructions only.");
	puts("If it doesn't fit in 16 bits, it doesn't run.");

	for (size_t i = 0; i < BUFFER_SIZE; )
	{
		size_t read = fread(&buf[i], 1, BUFFER_SIZE - i, stdin);
		if (read == 0)
		{
			puts("Error reading from stdin");
			return 1;
		}
		i += read;
	}

	for (size_t i = 0; i < BUFFER_SIZE; i += 2)
	{
		if ((buf[i] & 3) == 3)
		{
			puts("Compressed instructions only! Try again.");
			return 1;
		}
	}

	((void (*)())buf)();
    return 0;
}
