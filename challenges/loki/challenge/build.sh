#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/reverseme"
    "./build/challenge"
    "/lib/x86_64-linux-gnu/libc.so.6"
    "/lib64/ld-linux-x86-64.so.2"
)

# Build binaries
cd src
make clean
# Use default Makefile (production with anti-debugging enabled)
make
cd ..
cp src/reverseme build/
cp src/challenge build/

python3 src/update_checksum.py build/reverseme

# Copy required files to handout
mkdir -p handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
