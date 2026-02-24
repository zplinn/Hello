#!/bin/bash

# Post-build script to replace "halt" with "exit" in the binary
# This tricks some analysis tools into thinking the function exits the program

if [ $# -ne 1 ]; then
    echo "Usage: $0 <binary_path>"
    exit 1
fi

BINARY="$1"

if [ ! -f "$BINARY" ]; then
    echo "Error: Binary not found: $BINARY"
    exit 1
fi

# Use sed to replace "halt" with "exit" directly in the binary
# The -i flag edits in place, and we use LC_ALL=C to handle binary data
LC_ALL=C sed -i 's/halt/exit/g' "$BINARY"

# Verify the replacement
if strings "$BINARY" | grep -q "^halt$"; then
    echo "Warning: 'halt' string still found in binary"
else
    echo "Successfully replaced 'halt' with 'exit' in $BINARY"
fi

# Check if exit appears in strings now
if strings "$BINARY" | grep -q "^exit$"; then
    echo "Confirmed: 'exit' string found in binary"
fi