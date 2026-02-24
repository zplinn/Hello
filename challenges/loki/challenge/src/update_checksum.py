#!/usr/bin/env python3
import struct
import sys
from elftools.elf.elffile import ELFFile

def calculate_checksum(data):
    """Calculate the same checksum as memory_coherency_check()"""
    sum_val = 0
    for byte in data:
        sum_val = ((sum_val << 1) | (sum_val >> 31)) & 0xFFFFFFFF  # Rotate left
        sum_val ^= byte
    return sum_val

def update_checksum(binary_path):
    """Update the expected_checksum value in the binary"""
    # Read the binary
    with open(binary_path, 'rb') as f:
        data = bytearray(f.read())
    
    # Parse ELF to find .text section
    with open(binary_path, 'rb') as f:
        elffile = ELFFile(f)
        
        # The C code checksums from start of ELF to reset_configuration + 100
        # Start is always 0 (beginning of file)
        start_offset = 0
        
        # Find the symbol table
        symtab = elffile.get_section_by_name('.symtab')
        if not symtab:
            print("Error: Could not find symbol table")
            return False
        
        # Find reset_configuration symbol
        reset_config_addr = None
        for symbol in symtab.iter_symbols():
            if symbol.name == 'reset_configuration':
                reset_config_addr = symbol['st_value']
                break
        
        if reset_config_addr is None:
            print("Error: Could not find reset_configuration symbol")
            return False
        
        # Since we're working with file offsets, not memory addresses,
        # we need to convert the virtual address to file offset
        # For a typical ELF, code starts at 0x1000 file offset
        # but let's find the actual offset
        text_section = elffile.get_section_by_name('.text')
        if not text_section:
            print("Error: Could not find .text section")
            return False
        
        # Convert virtual address to file offset
        # reset_config_addr is relative to the load address
        # We need to find which section it's in
        file_offset = None
        for section in elffile.iter_sections():
            if (section['sh_addr'] <= reset_config_addr < 
                section['sh_addr'] + section['sh_size']):
                # Found the section containing this address
                offset_in_section = reset_config_addr - section['sh_addr']
                file_offset = section['sh_offset'] + offset_in_section
                break
        
        if file_offset is None:
            print("Error: Could not convert address to file offset")
            return False
        
        # End is reset_configuration function start
        end_offset = file_offset
        
        # Get data from start of file to end marker
        checksum_data = data[start_offset:end_offset]
        
        # Calculate checksum
        checksum = calculate_checksum(checksum_data)
        print(f"Calculated checksum: 0x{checksum:08x} (size: {len(checksum_data)} bytes)")
        
        # Find .checksum_data section
        checksum_section = elffile.get_section_by_name('.checksum_data')
        if not checksum_section:
            print("Error: Could not find .checksum_data section")
            return False
        
        # Update checksum in .checksum_data section
        cs_offset = checksum_section['sh_offset']
        cs_size = checksum_section['sh_size']
        
        # Find the expected_checksum variable (should be first 4 bytes of section)
        if cs_size >= 4:
            # Write the new checksum
            data[cs_offset:cs_offset+4] = struct.pack('<I', checksum)
            
            # Write back the modified binary
            with open(binary_path, 'wb') as f:
                f.write(data)
            
            print(f"Updated checksum at offset 0x{cs_offset:x}")
            return True
        else:
            print("Error: .checksum_data section too small")
            return False

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <binary_path>")
        sys.exit(1)
    
    binary_path = sys.argv[1]
    
    if update_checksum(binary_path):
        print("Checksum updated successfully")
    else:
        print("Failed to update checksum")
        sys.exit(1)