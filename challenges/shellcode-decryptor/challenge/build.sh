#!/bin/bash

set -e

HANDOUT_FILES=(
    "./src/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

scc --platform linux --arch x64 --stdout ./src/payload.c | python3 -c 'import sys; sys.stdout.buffer.write(bytes([x ^ 0x92 for x in sys.stdin.buffer.read()]))' | xxd -i > ./src/payload.h
gcc -o src/challenge ./src/challenge.c

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
