#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
    "/usr/lib/x86_64-linux-gnu/libpng16.so.16"
)

# Build binaries
gcc src/challenge.c -O0 -fno-stack-protector -Wall -o build/challenge -lpng -Wl,-rpath='$ORIGIN'

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
