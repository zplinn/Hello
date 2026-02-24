#!/usr/bin/env python3

import struct
import sys
import random
from Crypto.Cipher import ARC4


seed = int(sys.argv[1])
size = int(sys.argv[2])
flag = sys.argv[3]
outpath = sys.argv[4]

random.seed(seed)

data = [random.randint(-128, 127) for _ in range(size)]

def s2ub(x):
    return struct.unpack('<B', struct.pack('<b', x))[0]

def max_subarray(numbers):
    """Find the largest sum of any contiguous subarray."""
    best_sum = None
    best_start = None
    best_end = None

    current_sum = 0
    current_start = 0
    current_end = 0
    for idx, x in enumerate(numbers):
        if (current_sum + x) > x:
            current_sum = current_sum + x
            current_end = idx
        else:
            current_sum = x
            current_start = idx
            current_end = idx
        #current_sum = max(x, current_sum + x)

        if best_sum == None or current_sum > best_sum:
            best_start = current_start
            best_end = current_end
            best_sum = current_sum
        #best_sum = current_sum if current_sum == None else max(best_sum, current_sum)
    return best_sum, best_start, best_end

#print(data)
best_sum, best_start, best_end = max_subarray(data)
key_material = bytes([s2ub(x) for x in data[best_start:best_end+1]])

print(f'Start: {best_start}, end: {best_end}')
print(f'Key length: {len(key_material)}')

keyvals = [0]*16
for idx, x in enumerate(key_material):
    keyvals[idx % len(keyvals)] = (keyvals[idx % len(keyvals)] + x) & 0xFF
key = bytes(keyvals)

print(f'Key: {key.hex()}')

rc4 = ARC4.new(bytes(key))
encrypted_flag = rc4.encrypt(flag.encode() + b'\0')

with open(outpath, 'w') as fout:
    fout.write('#include "data.h"\n')
    fout.write(f'size_t data_len = {len(data)};\n')
    data_c = ','.join(f'{x:#04x}' for x in data)
    fout.write(f'int8_t data[] = {{ {data_c} }};\n')

    fout.write(f'size_t flag_enc_len = {len(encrypted_flag)};\n')
    encrypted_flag_c = ','.join(f'{x:#04x}' for x in encrypted_flag)
    fout.write(f'uint8_t flag_enc[] = {{ {encrypted_flag_c} }};\n')
