#include <stdio.h>
#include <stdlib.h>

void win(void)
{
    system("/bin/sh");
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void outputRun(char* out, size_t* outLen, char byte, size_t len)
{
	out[(*outLen)++] = (char)len;
	out[(*outLen)++] = byte;
}

void outputBytes(char* out, size_t* outLen, char* in, size_t start, size_t end)
{
	out[(*outLen)++] = 0x80 | (char)(end - start);
	for (size_t i = start; i < end; i++)
		out[(*outLen)++] = in[i];
}

size_t compress(char* in, size_t inLen, char* out)
{
	size_t outLen = 0;
	size_t nonRunStart = 0;
	size_t i;
	for (i = 0; i < inLen; )
	{
		char byte = in[i];
		size_t j;
		for (j = 1; i + j < inLen && j < 0x7f; j++)
		{
			if (in[i + j] != byte)
				break;
		}

		if (j > 1)
		{
			// Because this writes out a run length of 2, breaking up any output that is not a run,
			// this can cause the output to be much larger than the input. For example, the input
			// 01 01 02 03 03 04 will compress to 02 01 81 02 02 03 81 04, which is an input
			// of 6 bytes and an output of 8 bytes. This is 33% growth that can be repeated as
			// many times as desired.
			if (nonRunStart < i)
				outputBytes(out, &outLen, in, nonRunStart, i);
			outputRun(out, &outLen, byte, j);
			i += j;
			nonRunStart = i;
		}
		else
		{
			i++;
			if (i - nonRunStart >= 0x7f)
			{
				outputBytes(out, &outLen, in, nonRunStart, i);
				nonRunStart = i;
			}
		}
	}

	if (nonRunStart < i)
		outputBytes(out, &outLen, in, nonRunStart, i);

	return outLen;
}

void chunkedCompress(char* in, size_t inLen, void* writeContext,
	void (*write)(void*, char*, size_t, size_t),
	void (*finish)(void*, size_t))
{
	// Buffer of the "ideal" worst case size from the overhead of non-run length bytes. Real worst case is
	// much worse (33% increase).
	char outBuffer[1024 + ((1024 + 0x7f) / 0x7f)];

	size_t outOffset = 0;
	for (size_t i = 0; i < inLen; )
	{
		size_t chunkSize = 1024;
		if ((inLen - i) < chunkSize)
			chunkSize = inLen - i;

		size_t compressedSize = compress(in + i, chunkSize, outBuffer);
		write(writeContext, outBuffer, compressedSize, outOffset);

		i += chunkSize;
		outOffset += compressedSize;
	}

	finish(writeContext, outOffset);
}

void hexWrite(void* context, char* data, size_t len, size_t offset)
{
	for (size_t i = 0; i < len; i++)
	{
		size_t totalOffset = offset + i;
		if ((totalOffset & 0xf) == 0)
		{
			if (totalOffset != 0)
				printf("\n");
			printf("%.08zx  ", totalOffset);
		}
		printf("%.2x ", (unsigned char)data[i]);
	}
}

void hexFinish(void* context, size_t size)
{
	size_t* inputLen = (size_t*)context;
	printf("\nInput length %zd, output length %zd\n", *inputLen, size);
}

int main(int argc, char** argv, char** envp)
{
    init();

	printf("Free leaks!\n%p\n\n", chunkedCompress);

    puts("Length of input in bytes: ");
	char lenStr[32];
	if (!fgets(lenStr, sizeof(lenStr), stdin))
		return 1;
	size_t len = strtoul(lenStr, NULL, 10);

	char* buf = (char*)malloc(len);

	puts("Send bytes of input now.");

	for (size_t i = 0; i < len; )
	{
		size_t read = fread(buf + i, 1, sizeof(buf[i]), stdin);
		if (read == 0)
			return 1;
		i += read;
	}

	puts("Compressed:");
	chunkedCompress(buf, len, &len, hexWrite, hexFinish);
    return 0;
}
