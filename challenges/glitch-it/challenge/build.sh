#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
    "/usr/lib/libcapstone.so.5"
)

# Build binaries
gcc src/challenge.c -g -O0 -Wall -fno-stack-protector -o build/challenge -lcapstone

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
