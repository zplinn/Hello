@ upward_stack_ctf.s
@ ARM Assembly CTF with ONLY upward-growing stack
@ No push/pop - only manual stack operations in reverse
@ Compile: as -o upward_stack_ctf.o upward_stack_ctf.s
@ Link: ld -o upward_stack_ctf upward_stack_ctf.o

.global _start

.section .data
    @ Messages
    banner:     .ascii "=== Alternate Universe===\n"
    banner_len = . - banner
    
    username_prompt:     .ascii "Enter username: "
    username_prompt_len = . - username_prompt
    
    password_prompt:     .ascii "Enter password: "
    password_prompt_len = . - password_prompt
    
    main_msg:   .ascii "[main] Starting...\n"
    main_len = . - main_msg
    
    foo_msg:    .ascii "[foo] Handling username\n"
    foo_len = . - foo_msg
    
    bar_msg:    .ascii "[bar] Handling password\n"
    bar_len = . - bar_msg
    
    safe_msg:   .ascii "[main] Exited safely\n"
    safe_len = . - safe_msg
    
    canary_warn:   .ascii "Careful! Something bad almost happened...\n"
    canary_warn_len = . - canary_warn

.section .bss
    @ Stack space that grows upward (64KB)
    .align 4
    stack_bottom:
    .space 0x10000
    stack_top:

.section .text

@ Syscall numbers for ARM
.equ SYS_exit,  1
.equ SYS_read,  3
.equ SYS_write, 4
.equ STDOUT,    1
.equ STDIN,     0

@ Macro to print a message
.macro print_msg msg, len
    mov r0, #STDOUT
    ldr r1, =\msg
    mov r2, #\len
    mov r7, #SYS_write
    svc #0
.endm

@ Upward stack operations (opposite of normal)
@ Normal push: str rX, [sp, #-4]!  (pre-decrement)
@ Upward push: str rX, [sp], #4    (post-increment)
@ Normal pop:  ldr rX, [sp], #4    (post-increment)  
@ Upward pop:  ldr rX, [sp, #-4]!  (pre-decrement)

_start:
    @ Initialize stack pointer to bottom (grows UP)
    ldr sp, =stack_bottom
    
    @ Print banner
    print_msg banner, banner_len
    
    @ Jump to main
    b main

@ Main function
main:
    @ Save lr on upward stack (post-increment sp)
    str lr, [sp], #4    @ sp moves UP after store
    
    @ Print main message
    print_msg main_msg, main_len
    
    @ Allocate space for main's local buffer (32 bytes)
    @ In upward stack, we ADD to allocate
    add sp, sp, #32
    
    @ Call foo - lr will contain return address
    bl foo
    
    @ Deallocate main's buffer
    sub sp, sp, #32
    
    @ Print safe exit message
    print_msg safe_msg, safe_len
    
    @ Restore lr from upward stack (pre-decrement sp)
    ldr lr, [sp, #-4]!  @ sp moves DOWN before load
    
    @ Exit program
    mov r0, #0
    mov r7, #SYS_exit
    svc #0

@ Foo function
foo:
    @ Save lr on upward stack
    str lr, [sp], #4
    
    @ Save frame pointer
    str r11, [sp], #4
    mov r11, sp         @ Frame pointer points after saved regs
    
    @ Print foo message
    print_msg foo_msg, foo_len
    
    @ Allocate space for:
    @ 1. Username buffer (64 bytes)
    mov r4, sp          @ Save username buffer address
    add sp, sp, #64
    
    @ 2. Password buffer (32 bytes)
    mov r5, sp          @ Save password buffer address
    add sp, sp, #32
    
    @ 3. Saved "return address" location (4 bytes)
    @ This is what we want to be overwritten
    add sp, sp, #4
    
    @ Store safe return point at sp-4 (our "return address")
    adr r0, foo_safe_return
    str r0, [sp, #-4]   @ Store at top of our allocations
    
    @ Get username input
    print_msg username_prompt, username_prompt_len
    
    @ Read username (up to 64 bytes)
    mov r0, #STDIN
    mov r1, r4          @ Username buffer address
    mov r2, #64
    mov r7, #SYS_read
    svc #0
    
    @ Call bar with password buffer address
    mov r0, r5          @ Pass password buffer as argument
    bl bar
    
    @ Load potentially corrupted return address
    ldr r0, [sp, #-4]   @ This could be overwritten to win!
    
    @ Restore stack pointer
    mov sp, r11
    
    @ Restore saved registers from upward stack
    ldr r11, [sp, #-4]!
    ldr lr, [sp, #-4]!
    
    @ Jump to loaded address (potentially hijacked)
    bx r0

foo_safe_return:
    @ Normal return path
    bx lr

@ Bar function - contains vulnerability
bar:
    @ Save registers on upward stack
    str lr, [sp], #4
    str r4, [sp], #4
    str r5, [sp], #4
    str r6, [sp], #4
    
    @ Write stack canary (4-byte ASCII value "Live")
    mov r3, #0x65       @ 'e'
    orr r3, r3, #0x7600 @ 'v' << 8
    orr r3, r3, #0x690000 @ 'i' << 16
    orr r3, r3, #0x4C000000 @ 'L' << 24
    str r3, [sp], #4    @ Store canary and increment sp
    
    mov r4, r0          @ Save password buffer address (32 bytes)
    
    @ Print bar message  
    print_msg bar_msg, bar_len
    
    @ Get password input
    print_msg password_prompt, password_prompt_len
    
    @ VULNERABILITY: Read up to 128 bytes into 32-byte buffer
    mov r0, #STDIN
    mov r1, r4          @ Destination buffer (only 32 bytes!)
    mov r2, #128        @ Read up to 128 bytes
    mov r7, #SYS_read
    svc #0
    
    @ If we read > 32 bytes, we overwrite:
    @ +32: Start of foo's saved return address!
    
    @ Check canary before returning
    ldr r3, [sp, #-4]!  @ Load canary value and decrement sp
    mov r2, #0x65       @ 'e'
    orr r2, r2, #0x7600 @ 'v' << 8
    orr r2, r2, #0x690000 @ 'i' << 16
    orr r2, r2, #0x4C000000 @ 'L' << 24
    cmp r3, r2
    bne canary_fail     @ If canary doesn't match, exit
    
    @ Restore registers from upward stack
    ldr r6, [sp, #-4]!
    ldr r5, [sp, #-4]!
    ldr r4, [sp, #-4]!
    ldr lr, [sp, #-4]!
    
    bx lr

canary_fail:
    @ Canary was corrupted - print warning and exit
    print_msg canary_warn, canary_warn_len
    mov r0, #1          @ Exit with error code
    mov r7, #SYS_exit
    svc #0

@ Win function - target for exploitation
win:
    @ Execute /bin/sh
    ldr r0, =shell_path     @ First argument: path to /bin/sh
    mov r1, #0              @ Second argument: argv (NULL)
    mov r2, #0              @ Third argument: envp (NULL)
    mov r7, #11             @ SYS_execve
    svc #0
    
    @ If execve fails, exit
    mov r0, #1
    mov r7, #SYS_exit
    svc #0

.section .data
    shell_path: .asciz "/bin/sh"

@ Stack layout when bar is called (all growing UPWARD):
@ 
@ stack_bottom:
@ [main's lr - 4 bytes]
@ [main's buffer - 32 bytes]
@ [foo's lr - 4 bytes]  
@ [foo's r11 - 4 bytes]
@ [foo's username buffer - 64 bytes]
@ [foo's password buffer - 32 bytes] <- bar reads HERE
@ [foo's return addr - 4 bytes] <- TARGET at offset +32
@ [bar's lr - 4 bytes]
@ [bar's r4-r6 - 12 bytes]
@ [bar's canary - 4 bytes] <- "Live" canary value
@ sp points here ->
@
@ Overflow distance: 32 bytes to overwrite return address
@ Note: Overflowing further will corrupt the canary and cause warning + exit
