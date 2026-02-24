#include "check.h"
int check(uint8_t* flag, size_t len) {
	if((VM_MUL(flag[2],  197)&0xFF) != 206) { return 0; }
	if((VM_SUB(flag[13],  61)&0xFF) != 241) { return 0; }
	if((VM_SUB(flag[3],  185)&0xFF) != 172) { return 0; }
	if((VM_ADD(flag[6],  187)&0xFF) != 1) { return 0; }
	if((VM_MUL(flag[20],  209)&0xFF) != 18) { return 0; }
	if((VM_MUL(flag[7],  217)&0xFF) != 67) { return 0; }
	if((VM_SUB(flag[25],  101)&0xFF) != 250) { return 0; }
	if((VM_MUL(flag[22],  9)&0xFF) != 20) { return 0; }
	if((VM_ADD(flag[30],  157)&0xFF) != 26) { return 0; }
	if((VM_MUL(flag[4],  201)&0xFF) != 155) { return 0; }
	if((VM_MUL(flag[10],  207)&0xFF) != 122) { return 0; }
	if((VM_SUB(flag[9],  179)&0xFF) != 175) { return 0; }
	if((VM_SUB(flag[12],  9)&0xFF) != 106) { return 0; }
	if((VM_ADD(flag[8],  205)&0xFF) != 60) { return 0; }
	if((VM_ADD(flag[17],  135)&0xFF) != 250) { return 0; }
	if((VM_MUL(flag[11],  159)&0xFF) != 171) { return 0; }
	if((VM_ADD(flag[23],  101)&0xFF) != 217) { return 0; }
	if((VM_ADD(flag[26],  161)&0xFF) != 4) { return 0; }
	if((VM_MUL(flag[0],  187)&0xFF) != 132) { return 0; }
	if((VM_ADD(flag[14],  127)&0xFF) != 231) { return 0; }
	if((VM_MUL(flag[18],  35)&0xFF) != 253) { return 0; }
	if((VM_MUL(flag[15],  161)&0xFF) != 191) { return 0; }
	if((VM_SUB(flag[1],  85)&0xFF) != 20) { return 0; }
	if((VM_MUL(flag[21],  27)&0xFF) != 167) { return 0; }
	if((VM_SUB(flag[24],  53)&0xFF) != 68) { return 0; }
	if((VM_MUL(flag[29],  23)&0xFF) != 180) { return 0; }
	if((VM_SUB(flag[27],  171)&0xFF) != 196) { return 0; }
	if((VM_MUL(flag[28],  117)&0xFF) != 187) { return 0; }
	if((VM_MUL(flag[16],  235)&0xFF) != 99) { return 0; }
	if((VM_MUL(flag[5],  105)&0xFF) != 116) { return 0; }
	if((VM_ADD(flag[19],  23)&0xFF) != 135) { return 0; }
	return 1;
}
