#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <png.h>

typedef struct
{
	char* buf;
	size_t cur;
	size_t len;
} ReadContext;

void png_read_data(png_structp png_ptr, png_bytep data, png_size_t length) {
	ReadContext* rc = (ReadContext*)png_get_io_ptr(png_ptr);
	if (rc->cur + length > rc->len || rc->cur + length < rc->cur)
	{
		png_error(png_ptr, "PNG image truncated");
		return;
	}

    memcpy(data, &rc->buf[rc->cur], length);
    rc->cur += length;
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main(int argc, char** argv, char** envp)
{
    init();

	puts("Length of input in bytes: ");
	unsigned int len = 0;

	char lenBuf[64];
	if (!fgets(lenBuf, 64, stdin))
	{
		puts("Bad length\n");
		return 1;
	}
	len = strtoul(lenBuf, NULL, 10);

	if (len < 32)
	{
		puts("Too small!\n");
		return 1;
	}
	if (len > 65536)
	{
		puts("Too large!\n");
		return 1;
	}

	char* buf = (char*)mmap(NULL, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	for (size_t i = 0; i < len; )
	{
		size_t read = fread(&buf[i], 1, len - i, stdin);
		if (read == 0)
		{
			puts("Error reading from stdin");
			return 1;
		}
		i += read;
	}

	puts("Validating the image...");

    // Validate PNG signature
    if (png_sig_cmp((png_const_bytep)buf, 0, 8) != 0) {
        puts("Invalid PNG signature");
        return 1;
    }

    // Initialize PNG read struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        puts("Failed to create PNG read struct");
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        puts("Failed to create PNG info struct");
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        puts("Invalid PNG data");
        return 1;
    }

    // Set up custom read function to read from memory
	ReadContext rc;
	rc.buf = buf;
	rc.cur = 0;
	rc.len = len;
    png_set_read_fn(png_ptr, &rc, (png_rw_ptr)png_read_data);

    // Read PNG header
    png_read_info(png_ptr, info_ptr);

	// Get image info
	png_uint_32 width, height;
	int bit_depth, color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	// Allocate memory for row pointers
	png_bytep* row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(png_uint_32 y = 0; y < height; y++) {
	    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}

	// Read image data
	png_read_image(png_ptr, row_pointers);

	// Free row pointers
	for(png_uint_32 y = 0; y < height; y++) {
	    free(row_pointers[y]);
	}
	free(row_pointers);

    // Cleanup
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	puts("Executing the image...");

	char* workarea = (char*)mmap((void*)0x50000000, 0x10000000LL, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (workarea == (void*)-1)
	{
		puts("Work area failed to allocate");
		return 1;
	}

	__asm__(
	    "mov %0, %%rax\n\t"
	    "call *%1\n\t"
	    :
	    : "r" (workarea), "r" (buf)
	    : "rax"
	);
    return 0;
}
