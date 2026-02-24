#include "rc4.h"

#define N 256

static void swap(unsigned char *a, unsigned char *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

static int KSA(uint8_t *rc4_state, uint8_t *key, size_t keylen)
{
    int j = 0;

    for (int i = 0; i < N; i++)
    {
        rc4_state[i] = i;
    }

    for (int i = 0; i < N; i++)
    {
        j = (j + rc4_state[i] + key[i % keylen]) % N;

        swap(&rc4_state[i], &rc4_state[j]);
    }

    return 0;
}

static int PRGA(uint8_t *rc4_state, uint8_t *plaintext, uint8_t *ciphertext, size_t len)
{

    int i = 0;
    int j = 0;

    for (size_t n = 0; n < len; n++)
    {
        i = (i + 1) % N;
        j = (j + rc4_state[i]) % N;

        swap(&rc4_state[i], &rc4_state[j]);
        int rnd = rc4_state[(rc4_state[i] + rc4_state[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];
    }

    return 0;
}

int RC4(uint8_t *key, size_t keylen, uint8_t *plaintext, uint8_t *ciphertext, size_t len)
{

    unsigned char rc4_state[N];
    KSA(rc4_state, key, keylen);

    PRGA(rc4_state, plaintext, ciphertext, len);

    return 0;
}