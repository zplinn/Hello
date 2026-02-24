#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define STACK_SIZE 512
#define MAX_OPS 10000

// #define DEBUG
#include "code.h"

#include <exceptions4c.h>

typedef struct
{
    size_t vm_ip;
    size_t vm_sp;
    uint8_t *code;
    int32_t stack[STACK_SIZE];
} vm_state;

typedef struct
{
    uint8_t opcode;
    const struct e4c_exception_type x;
} op_mapping;

// TODO: obfuscate names

op_mapping Y[] = {
    {0, {NULL, "HLT"}},
    {0x01 ^ 0x55, {NULL, "OP_DUP"}},
    {0x02 ^ 0x55, {NULL, "OP_DP2"}},
    {0x03 ^ 0x55, {NULL, "OP_DRP"}},
    {0x04 ^ 0x55, {NULL, "OP_SWP"}},
    {0x05 ^ 0x55, {NULL, "OP_PSH"}},

    {0x06 ^ 0x55, {NULL, "OP_ADD"}},
    {0x07 ^ 0x55, {NULL, "OP_SUB"}},
    {0x08 ^ 0x55, {NULL, "OP_MUL"}},
    {0x09 ^ 0x55, {NULL, "OP_XOR"}},

    {0x0A ^ 0x55, {NULL, "OP_CMP"}},
    {0x0B ^ 0x55, {NULL, "OP_JMP"}},
    {0x0C ^ 0x55, {NULL, "OP_JEQ"}},
    {0x0D ^ 0x55, {NULL, "OP_JNE"}},
};

int32_t vm_pop(vm_state *state)
{
    int32_t val = state->stack[--state->vm_sp];
#ifdef DEBUG
    printf("pop %#x\n", val);
#endif
    return val;
}

void vm_push(vm_state *state, int32_t val)
{
    state->stack[state->vm_sp++] = val;
}

uint8_t vm_operand_u8(vm_state *state)
{
    uint8_t val = state->code[state->vm_ip];
    state->vm_ip += sizeof(val);
    return val;
}

int8_t vm_operand_i8(vm_state *state)
{
    int8_t val = *(int8_t *)&state->code[state->vm_ip];
    state->vm_ip += sizeof(val);
    return val;
}
int16_t vm_operand_i16(vm_state *state)
{
    int16_t val = *(int16_t *)&state->code[state->vm_ip];
    state->vm_ip += sizeof(val);
    return val;
}

int32_t vm_operand_i32(vm_state *state)
{
    int32_t val = *(int32_t *)&state->code[state->vm_ip];
    state->vm_ip += sizeof(val);
    return val;
}

void vm_jmp_rel8(vm_state *state, int8_t val)
{
    state->vm_ip += val;
}
void vm_jmp_rel16(vm_state *state, int16_t val)
{
    state->vm_ip += val;
}

void fetch_opcode(vm_state *state)
{
#ifdef DEBUG
    printf("IP: %ld, SP: %ld\n", state->vm_ip, state->vm_sp);
    printf("Stack:");
    for (size_t i = 0; i < state->vm_sp; i++)
    {
        printf("%x,", state->stack[i]);
    }
    printf("\n");
#endif
    uint8_t opcode = state->code[state->vm_ip++];
    if (opcode == 0)
    {
#ifdef DEBUG
        printf("OP_HLT\n");
#endif
        return;
    }
    for (size_t i = 0; i < sizeof(Y); i++)
    {
        if (Y[i].opcode == opcode)
        {
            THROW(Y[i].x, "Oh no");
        }
    }
}

void init_vm(vm_state *state, uint8_t *code)
{
    state->vm_ip = 0;
    state->vm_sp = 0;
    state->code = code;
    memset(state->stack, 0, sizeof(state->stack));
}

void init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int main()
{
    init();
    vm_state state;
    init_vm(&state, code);

    printf("Flag: ");
    char flag[256];
    if (fgets(flag, flaglen + 1, stdin) == NULL)
    {
        printf("Failed to read input\n");
        return 1;
    }
    flag[strcspn(flag, "\n")] = 0;
    if(strlen(flag) != flaglen) {
        return 1;
    }
    for (size_t i = 0; i < strlen(flag); i++)
    {
        vm_push(&state, flag[i]);
    }

    TRY
    {
        // THROW(Y[0].x, "Step 1");
        fetch_opcode(&state);
    }
    CATCH(Y[1].x) // DUP
    {
#ifdef DEBUG
        printf("OP_DUP\n");
#endif
        int32_t val = vm_pop(&state);
        vm_push(&state, val);
        vm_push(&state, val);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[2].x) // DUP2
    {
#ifdef DEBUG
        printf("OP_DUP2\n");
#endif
        int32_t val1 = vm_pop(&state);
        int32_t val2 = vm_pop(&state);
        vm_push(&state, val2);
        vm_push(&state, val1);
        vm_push(&state, val2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[3].x) // DRP
    {
#ifdef DEBUG
        printf("OP_DRP\n");
#endif
        vm_pop(&state);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[4].x) // SWP
    {
#ifdef DEBUG
        printf("OP_SWP\n");
#endif
        int32_t val1 = vm_pop(&state);
        int32_t val2 = vm_pop(&state);
        vm_push(&state, val1);
        vm_push(&state, val2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[5].x) // PSH
    {
        int32_t val = vm_operand_i32(&state);
#ifdef DEBUG
        printf("OP_PSH %#x\n", val);
#endif
        vm_push(&state, val);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[6].x) // ADD
    {
#ifdef DEBUG
        printf("OP_ADD\n");
#endif

        int32_t op1 = vm_pop(&state);
        int32_t op2 = vm_pop(&state);
        vm_push(&state, op1 + op2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[7].x) // SUB
    {
#ifdef DEBUG
        printf("OP_SUB\n");
#endif

        int32_t op1 = vm_pop(&state);
        int32_t op2 = vm_pop(&state);
        vm_push(&state, op1 - op2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[8].x) // MUL
    {
#ifdef DEBUG
        printf("OP_MUL\n");
#endif

        int32_t op1 = vm_pop(&state);
        int32_t op2 = vm_pop(&state);
        vm_push(&state, op1 * op2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[9].x) // XOR
    {
#ifdef DEBUG
        printf("OP_XOR\n");
#endif

        int32_t op1 = vm_pop(&state);
        int32_t op2 = vm_pop(&state);
        vm_push(&state, op1 ^ op2);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }

    CATCH(Y[0xA].x) // CMP
    {

        int32_t op1 = vm_pop(&state);
        int32_t op2 = vm_pop(&state);
        uint8_t res = op1 == op2;
#ifdef DEBUG
        printf("OP_CMP %x == %x (%x)\n", op1, op2, res);
#endif
        vm_push(&state, res);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[0xB].x) // JMP
    {
#ifdef DEBUG
        printf("OP_JMP\n");
#endif

        int16_t op1 = vm_operand_i16(&state);
        vm_jmp_rel16(&state, op1);

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[0xC].x) // JEQ
    {
#ifdef DEBUG
        printf("OP_JEQ\n");
#endif

        int32_t val = vm_pop(&state);
        int16_t op1 = vm_operand_i16(&state);
        if (val == 1)
        {
            vm_jmp_rel16(&state, op1);
        }

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }
    CATCH(Y[0xD].x) // JNE
    {

        int32_t val = vm_pop(&state);
        int16_t op1 = vm_operand_i16(&state);

#ifdef DEBUG
        printf("OP_JNE %x (%x)\n", op1, val);
#endif
        if (val == 0)
        {
            vm_jmp_rel16(&state, op1);
        }

        RETRY(MAX_OPS, Y[0].x, "HLT");
    }

    CATCH_ALL
    {
#ifdef DEBUG
        printf("OP_UNK\n");
#endif
    }

    if (state.stack[state.vm_sp] == 1)
    {
        printf("Correct! Congratulations!\n");
        return 0;
    }
    else
    {
        printf("Wrong! Try again!\n");
        return 1;
    }
}