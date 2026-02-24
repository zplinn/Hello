CC = gcc
CFLAGS = -Wall -O0 -fno-stack-protector -DENABLE_ANTIDEBUG -march=x86-64 -mtune=generic -fno-asynchronous-unwind-tables
CHALLENGE_LDFLAGS = -lcrypto

all: reverseme challenge

reverseme: reverseme.c
	$(CC) $(CFLAGS) -o reverseme reverseme.c -Wl,-T,checksum.ld
	./post-build.sh reverseme

challenge: challenge.c flag_hash.h
	$(CC) $(CFLAGS) -o challenge challenge.c $(CHALLENGE_LDFLAGS)

test: reverseme.c
	$(CC) $(CFLAGS) -DTEST_FLAG -o reverseme-test reverseme.c -Wl,-T,checksum.ld
	./post-build.sh reverseme-test

clean:
	rm -f reverseme challenge reverseme-test

.PHONY: all clean test