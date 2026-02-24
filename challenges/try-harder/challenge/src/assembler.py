#!/usr/bin/env python3

import ast
import struct
import random
import sys

flag = sys.argv[1]
seed = int(sys.argv[2])
outpath = sys.argv[3]

random.seed(seed)

"""
pseudo code:
for(i = 0; i < N; i += 1) {
    if(i == 0) {
        if(flag[0]*x1 != x2) {
            fail
        }
        continue
    }
    if(i == 1) {
        if(flag[1]+x1 != x2) {
            fail
        }
        continue
    }
    ...

}
"""

asm = """
;f0, f1, ..., fn
label start
;f0, f1, ..., fn, i
push 0
label loop
"""

chunk = """
label %s
;f0, f1, ..., fn, i, i
dup
;f0, f1, ..., fn, i, i, 0
push %#x
;f0, f1, ..., fn, i, r
cmp
;f0, f1, ..., fn, i
jne %s
;f0, f1, ..., fn-1, i, fn
swap
;f0, f1, ..., fn-1, i, fn, c0
push %#x
;f0, f1, ..., fn-1, i, fn*c0
%s
;f0, f1, ..., fn-1, i, fn*c0, 0x12
push %#x
;f0, f1, ..., fn-1, i, r
cmp
;f0, f1, ..., fn-1, i
jne fail
;f0, f1, ..., fn-1, i
jmp loopend
"""

asmend = """
label loopend
;f0, f1, ..., fn-1, i, 1
push 1
;f0, f1, ..., fn-1, i+1
add
;f0, f1, ..., fn-1, i+1, i+1
dup
;f0, f1, ..., fn-1, i+1, i+1, N
push %#x
;f0, f1, ..., fn-1, i+1, r
cmp
;f0, f1, ..., fn-1, i+1
jne loop
drop
; 1
push 1
; 1
hlt
label fail
push 0
; 0
hlt
"""

for chunk_idx, x in enumerate(flag.encode()[::-1]):
    op = random.choice(['add', 'sub', 'mul', 'xor'])
    val1 = random.randint(0, 0xff)
    match op:
        case 'add':
            val2 = x + val1
        case 'sub':
            val2 = val1 - x
        case 'mul':
            val2 = x * val1
        case 'xor':
            val2 = x ^ val1

    nextchunk = f'chunk_{chunk_idx+1}' if chunk_idx + 1 < len(flag) else 'loopend'
    asm += (chunk % (f'chunk_{chunk_idx}', chunk_idx, nextchunk, val1, op, val2))

asm += (asmend % len(flag))

"""
{0x01 ^ 0x55, {NULL, "OP_DUP"}},
{0x02 ^ 0x55, {NULL, "OP_DP2"}},
{0x03 ^ 0x55, {NULL, "OP_DRP"}},
{0x04 ^ 0x55, {NULL, "OP_SWP"}},
{0x05 ^ 0x55, {NULL, "OP_PSH"}},

{0x06 ^ 0x55, {NULL, "OP_ADD"}},
{0x07 ^ 0x55, {NULL, "OP_SUB"}},
{0x08 ^ 0x55, {NULL, "OP_MUL"}},
{0x09 ^ 0x55, {NULL, "OP_XOR"}},

{0x0A ^ 0x55, {NULL, "OP_CMP"}},
{0x0B ^ 0x55, {NULL, "OP_JMP"}},
{0x0C ^ 0x55, {NULL, "OP_JEQ"}},
{0x0D ^ 0x55, {NULL, "OP_JNE"}},
"""

opcode_key = 0x55

binary = bytearray()
labels = {}
placeholders = {}
for line in asm.splitlines():
    line = line.strip()
    if len(line) == 0:
        continue
    if line.startswith(';'):
        print(f'Comment: {line[1:]}')
        continue
    parts = line.split()
    match parts[0]:
        case 'label':
            label = parts[1]
            labels[label] = len(binary)
        
        case 'dup':
            binary.append(0x1 ^ opcode_key)
            print('dup')
        case 'dup2':
            binary.append(0x2 ^ opcode_key)
            print('dup2')
        case 'drop':
            binary.append(0x3 ^ opcode_key)
            print('drop')
        case 'swap':
            binary.append(0x4 ^ opcode_key)
            print('swap')
        
        case 'push':
            binary.append(0x5 ^ opcode_key)
            val = ast.literal_eval(parts[1])
            binary += struct.pack('<i', val)
            print(f'push {val:#x}')

        case 'add':
            binary.append(0x6 ^ opcode_key)
            print('add')
        case 'sub':
            binary.append(0x7 ^ opcode_key)
            print('sub')
        case 'mul':
            binary.append(0x8 ^ opcode_key)
            print('mul')
        case 'xor':
            binary.append(0x9 ^ opcode_key)
            print('xor')

        case 'cmp':
            binary.append(0xA ^ opcode_key)
            print('cmp')

        case 'jmp':
            binary.append(0xB ^ opcode_key)
            label = parts[1]
            placeholders[len(binary)] = label
            binary += struct.pack('<h', 0)
            print(f'jmp {label}')

        case 'jeq':
            binary.append(0xC ^ opcode_key)
            label = parts[1]
            placeholders[len(binary)] = label
            binary += struct.pack('<h', 0)
            print(f'jeq {label}')

        case 'jne':
            binary.append(0xD ^ opcode_key)
            label = parts[1]
            placeholders[len(binary)] = label
            binary += struct.pack('<h', 0)
            print(f'jne {label}')

        case 'hlt':
            binary.append(0)
            print('hlt')

        case _:
            print(f'Unknown op {parts[0]}')

print(binary.hex())

for src_offset, dst_label in placeholders.items():
    dst_offset = labels[dst_label]
    jmp_delta = dst_offset - src_offset - 2
    binary[src_offset:src_offset+2] = struct.pack('<h', jmp_delta) 

print(placeholders)
print(labels)

#print(binary.hex())

with open(outpath, 'w') as fout:
    fout.write('#include "code.h"\n')
    code_str = ', '.join(f'{x:#04x}' for x in binary)
    fout.write(f'uint8_t code[] = {{ {code_str} }};\n')
    fout.write(f'size_t codelen = {len(binary)};\n')
    fout.write(f'size_t flaglen = {len(flag)};\n')