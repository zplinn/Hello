#ifndef OBFUSCATION_H
#define OBFUSCATION_H

// Overlapping instruction macros to confuse disassemblers
// These create instruction sequences that can be interpreted differently
// depending on the entry point, breaking linear disassembly

// Variant 1: Jump over fake MOV instructions
#define OVERLAP_V1() \
    __asm__ volatile( \
        "jmp 1f;" \
        ".byte 0x48, 0x89, 0xc0;" /* fake: mov rax, rax */ \
        ".byte 0x48, 0x89, 0xdb;" /* fake: mov rbx, rbx */ \
        "1:" \
    )

// Variant 2: Backward jump pattern with embedded jump in immediate
#define OVERLAP_V2() \
    __asm__ volatile( \
        "push %rdx;" \
        "mov $0x909005eb90909090, %rdx;" \
        ".byte 0xeb;" \
        ".byte 0xfa;" /* jmp -6 to land after the 0xeb in mov */ \
        "ret;" \
        "pop %rdx;" \
    )

// Variant 3: Custom pattern with backward jump
#define OVERLAP_V3() \
    __asm__ volatile( \
        "push %rax;" \
        "mov $0x03eb353535353535, %rax;" \
        ".byte 0xeb;" \
        ".byte 0xfc;" /* jmp -4 */ \
        "ret;" \
        "pop %rax;" \
    )

// Variant 4: Fake function prologue
#define OVERLAP_V4() \
    __asm__ volatile( \
        "jmp 1f;" \
        ".byte 0x55;" /* push rbp */ \
        ".byte 0x48, 0x89, 0xe5;" /* mov rbp, rsp */ \
        ".byte 0x48, 0x83, 0xec, 0x20;" /* sub rsp, 0x20 */ \
        "1:" \
    )

// Macro for inserting fake code patterns that look like real functions
#define FAKE_FUNCTION_PATTERN() \
    __asm__ volatile( \
        "jmp 1f;" \
        ".align 16;" \
        ".byte 0x55;" /* push rbp */ \
        ".byte 0x48, 0x89, 0xe5;" /* mov rbp, rsp */ \
        ".byte 0x48, 0x8d, 0x3d, 0x00, 0x00, 0x00, 0x00;" /* lea rdi, [rip+0] */ \
        ".byte 0xe8, 0x00, 0x00, 0x00, 0x00;" /* call 0 */ \
        ".byte 0x5d;" /* pop rbp */ \
        ".byte 0xc3;" /* ret */ \
        "1:" \
    )

// Macro for creating opaque predicates (conditions that always evaluate the same)
// Note: We use global variables to prevent compiler optimization
extern volatile const int g_oz;
extern volatile const int g_oo;

#define OPAQUE_PREDICATE_TRUE() \
    ((g_oz * g_oz) == 0)

#define OPAQUE_PREDICATE_FALSE() \
    ((g_oo * g_oo) == 0)

// Macro for junk code insertion with misalignment
#define INSERT_JUNK() \
    __asm__ volatile( \
        ".byte 0x90;" /* nop */ \
        ".byte 0xeb, 0x01;" /* jmp +1 */ \
        ".byte 0x90;" /* this byte creates misalignment */ \
        ".byte 0x48, 0x31;" /* start of xor rax, rax */ \
        ".byte 0xc0;" /* complete the xor */ \
    )

#endif // OBFUSCATION_H