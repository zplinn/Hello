
#include "stage2.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

__attribute((__used__, __section__("text2")))
int stage2_check(uint8_t* input, size_t len);

__attribute((__used__, __section__("text2")))
uint64_t stage2_validate() {
    return 0x133713381339133A;
}

int stage2() {

    printf("Enter password: ");
    char password[32];
    if(fgets(password, sizeof(password)*sizeof(password[0]), stdin) == NULL) {
        return 0;
    }

    size_t passwordlen = strcspn(password, "\n");
    password[passwordlen] = 0;

    if(stage2_check(password, passwordlen) == 1) {
        puts("Correct! Congratulations!");
        return 1;
    } else {
        puts("Wrong!");
        return 0;
    }
}
int stage2_check(uint8_t* input, size_t len) {
	if((input[0] ^ 66) != 14) { return 0; }
	if((input[1] ^ 169) != 192) { return 0; }
	if((input[2] ^ 216) != 174) { return 0; }
	if((input[3] ^ 20) != 113) { return 0; }
	if((input[4] ^ 60) != 127) { return 0; }
	if((input[5] ^ 210) != 134) { return 0; }
	if((input[6] ^ 153) != 223) { return 0; }
	if((input[7] ^ 20) != 111) { return 0; }
	if((input[8] ^ 222) != 191) { return 0; }
	if((input[9] ^ 83) != 12) { return 0; }
	if((input[10] ^ 108) != 28) { return 0; }
	if((input[11] ^ 233) != 155) { return 0; }
	if((input[12] ^ 225) != 208) { return 0; }
	if((input[13] ^ 189) != 208) { return 0; }
	if((input[14] ^ 41) != 26) { return 0; }
	if((input[15] ^ 244) != 171) { return 0; }
	if((input[16] ^ 228) != 144) { return 0; }
	if((input[17] ^ 191) != 143) { return 0; }
	if((input[18] ^ 192) != 240) { return 0; }
	if((input[19] ^ 241) != 174) { return 0; }
	if((input[20] ^ 180) != 210) { return 0; }
	if((input[21] ^ 35) != 66) { return 0; }
	if((input[22] ^ 3) != 113) { return 0; }
	if((input[23] ^ 134) != 251) { return 0; }
return 1;
}
