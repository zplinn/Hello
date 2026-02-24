#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
python3 src/generate.py 1337 1000000 'LiveCTF{bl4zing_f4st_0h_y3ah}' src/data.c
gcc src/challenge.c src/data.c src/rc4.c -s -O2 -Wpedantic -Wall -o build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
