#!/usr/bin/env python3

import sys
import random

flag = sys.argv[1]
assert len(flag) < 32
outpath = sys.argv[2]

random.seed(flag.encode())

preamble = '''
#include "stage2.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

__attribute((__used__, __section__("text2")))
int stage2_check(uint8_t* input, size_t len);

__attribute((__used__, __section__("text2")))
uint64_t stage2_validate() {
    return 0x133713381339133A;
}

int stage2() {

    printf("Enter password: ");
    char password[32];
    if(fgets(password, sizeof(password)*sizeof(password[0]), stdin) == NULL) {
        return 0;
    }

    size_t passwordlen = strcspn(password, "\\n");
    password[passwordlen] = 0;

    if(stage2_check(password, passwordlen) == 1) {
        puts("Correct! Congratulations!");
        return 1;
    } else {
        puts("Wrong!");
        return 0;
    }
}
'''


with open(outpath, 'w') as fout:
    fout.write(preamble)
    fout.write('int stage2_check(uint8_t* input, size_t len) {\n');

    for idx, x in enumerate(flag.encode()):
        val1 = random.randint(0, 0xFF)
        val2 = x ^ val1
        fout.write(f'\tif((input[{idx}] ^ {val1}) != {val2}) {{ return 0; }}\n')

    fout.write('return 1;\n');
    fout.write('}\n');
