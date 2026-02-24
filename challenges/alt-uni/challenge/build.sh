#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
)

# Build binaries
# gcc src/challenge.c -O0 -fno-stack-protector -Wall -o build/challenge
arm-linux-gnueabi-as -o build/alt_uni.o src/alt_uni.s
arm-linux-gnueabi-ld -o build/challenge build/alt_uni.o
rm build/alt_uni.o

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
