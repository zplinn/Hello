#!/usr/bin/env python3

FLAG = "LiveCTF{reverse_engineering_the_trickster_away}"
XOR_KEY = 0x42

# XOR encode the flag
obfuscated = []
for char in FLAG:
    obfuscated.append(ord(char) ^ XOR_KEY)

# Print as C array
print("const unsigned char obfuscated_flag[] = {")
for i in range(0, len(obfuscated), 15):
    chunk = obfuscated[i:i+15]
    hex_values = ", ".join(f"0x{b:02x}" for b in chunk)
    if i + 15 < len(obfuscated):
        print(f"    {hex_values},")
    else:
        print(f"    {hex_values}")
print("};")

# Verify decoding
decoded = ""
for b in obfuscated:
    decoded += chr(b ^ XOR_KEY)
print(f"\nOriginal: {FLAG}")
print(f"Decoded:  {decoded}")
print(f"Match: {FLAG == decoded}")