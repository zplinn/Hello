/* Linker script section to add .checksum_data */
SECTIONS
{
    .checksum_data : {
        *(.checksum_data)
    }
}
INSERT AFTER .rodata;