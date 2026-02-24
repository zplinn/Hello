#!/bin/bash

set -e

HANDOUT_FILES=(
    "./build/challenge.exe"
)

# Build binaries
cd src
wine /tcc/tcc.exe -o /build/challenge.exe challenge.c -I/obfus.h/include
cd -

# Copy required files to handout
for f in ${HANDOUT_FILES[@]}; do
    cp $f handout/
done
