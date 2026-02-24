#!/usr/bin/env python3
import hashlib

FLAG = "LiveCTF{reverse_engineering_the_trickster_away}"
hash_obj = hashlib.sha256(FLAG.encode())
hash_hex = hash_obj.hexdigest()

print(f"Flag: {FLAG}")
print(f"SHA256: {hash_hex}")

# Generate C header file
with open("flag_hash.h", "w") as f:
    f.write(f'#define FLAG_HASH "{hash_hex}"\n')
    f.write(f'// SHA256 of: {FLAG}\n')