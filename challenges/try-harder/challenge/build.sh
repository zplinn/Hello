#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
python3 src/assembler.py 'LiveCTF{once-m0re-w1th-f33ling}' 1337 src/code.c
gcc -Wall -Wpedantic -s -O0 -Iexceptions4c/src -o build/challenge src/challenge.c src/code.c exceptions4c/lib/libexceptions4c.a

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
