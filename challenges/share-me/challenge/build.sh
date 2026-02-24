#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
python3 src/generate.py "LiveCTF{a_pr1m3_t00_far}" src/stage2.c
gcc src/challenge.c src/rc4.c src/stage2.c src/sha2-obf.c -O2 -s -Wall -o build/challenge_clean
gcc src/keygen.c src/sha2-obf.c -O2 -s -Wall -o keygen
python3 src/pack.py "pr1m3" build/challenge_clean build/challenge
chmod +x build/challenge

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
