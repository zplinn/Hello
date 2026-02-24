#!/usr/bin/env python3

import sys
import lief
from Crypto.Cipher import ARC4
import subprocess

username = sys.argv[1]
inpath = sys.argv[2]
outpath = sys.argv[3]

keygen = subprocess.Popen(['./keygen'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
keyhex, _ = keygen.communicate(input=(username + '\n').encode())
key = bytes.fromhex(keyhex.decode().strip())

elf = lief.ELF.parse(inpath)

print(f'Key = {key.hex()}')

with open(inpath, "rb") as fin:
    section = elf.get_section('text2')
    start, size = section.file_offset, section.original_size
    data = bytearray(fin.read())
    rc4 = ARC4.new(key)
    data[start:start+size] = rc4.encrypt(data[start:start+size])

with open(outpath, "wb") as fout:
    fout.write(data)
