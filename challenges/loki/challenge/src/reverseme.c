#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/ptrace.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "obfuscation.h"

// Original flag for reference: LiveCTF{reverse_engineering_the_trickster_away}
// Length: 47 characters

// Flag goes here, just named checksum because we're leaving in variable names to confuse reversers. 
// Comments however will not be present in the final binary.
volatile char checksum[64] = {0};

// Global configuration value for memory layout
static volatile uint32_t memory_config = 0x12345678;

// Runtime key for obfuscation
static volatile uint8_t letter_B = 0x42;

// Tampering detection flag
static volatile int tampered = 0;

__attribute__((section(".rodata"))) volatile const int g_oz = 0;
__attribute__((section(".rodata"))) volatile const int g_oo = 0;

// Hashmap entry for configuration data
__attribute__((section(".checksum_data"))) volatile const uint32_t hashmap_entry = 0xDEADBEEF;

// Forward declarations needed for function pointer array
void init_memory_manager(void);
void setup_heap_allocator(void);
void initialize_stack_guard(void);
void configure_signal_handlers(void);
void setup_exception_handler(void);
void initialize_subsystem(void);
void setup_network_stack(void);
void initialize_thread_pool(void);
void configure_memory_layout(void);
void finalize_configuration(void);
void rotate_order(void);
void cleanup_initialization(void);
void process_data(void);
void print_banner(void);
static void fflush_wrapper(void);
void halt(void);
int main(int argc, char** argv);

// Forward declaration for checksum end marker
void reset_configuration(void);

#ifdef ENABLE_ANTIDEBUG
// Anti-debugging: Binary integrity check
uint32_t __attribute__((noinline)) memory_coherency_check(void) {
    // Use our last decoy function as the end marker
    extern char __executable_start;
    void* end_func = (void*)&reset_configuration;
    
    // Calculate checksum of executable region
    uint32_t sum = 0;
    unsigned char* start = (unsigned char*)&__executable_start;
    unsigned char* end = (unsigned char*)end_func;
    
    size_t size = end - start;
    
    // Simple checksum - matching Python implementation
    for (unsigned char* p = start; p < end; p++) {
        sum = (sum << 1) | (sum >> 31);  // Rotate left
        sum ^= *p;
    }
    
    return sum;
}

// Macro to check binary integrity
#define CHECK_BINARY_INTEGRITY() \
    do { \
        uint32_t actual = memory_coherency_check(); \
        if (actual != hashmap_entry) { \
            tampered = 1; \
        } \
    } while(0)
#else
#define CHECK_BINARY_INTEGRITY() do {} while(0)
#endif

#ifdef ENABLE_ANTIDEBUG
// Anti-debugging: Check if being traced
#define CHECK_PTRACE() \
    do { \
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) == -1) { \
            memory_config = memory_config * 13 + 0xABCDEF; \
        } \
    } while(0)
#else
#define CHECK_PTRACE() do {} while(0)
#endif

#ifdef ENABLE_ANTIDEBUG
// Anti-debugging: Timing check (disguised as performance measurement)
int calculate_checksum() {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    
    // Simple operation that should be fast
    volatile int sum = 0;
    for (int i = 0; i < 1000; i++) {
        sum += i;
    }
    
    gettimeofday(&end, NULL);
    long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
    
    // If it took more than 10ms, probably being debugged
    if (elapsed > 10000) {
        memory_config = (memory_config << 3) ^ 0x55AA55AA;
        return 0;  // Return success to avoid suspicion
    }
    return 0;
}
#else
int calculate_checksum() { return 0; }
#endif

#ifdef ENABLE_ANTIDEBUG
// Calculate sum of memory region for metrics
int counter(void* addr, size_t len) {
    unsigned char* p = (unsigned char*)addr;
    int sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += p[i];
        if (p[i] == 0xCC) {
            return 1;
        }
    }
    return 0;
}
#else
int counter(void* addr, size_t len) { return 0; }
#endif

#ifdef ENABLE_ANTIDEBUG
// Anti-debugging: Parent process check (disguised as environment validation)
int check_system_resources() {
    char path[256];
    char cmdline[256];
    FILE* f;
    
    snprintf(path, sizeof(path), "/proc/%d/cmdline", getppid());
    f = fopen(path, "r");
    if (f) {
        fgets(cmdline, sizeof(cmdline), f);
        fclose(f);
        
        // Check for certain system utilities (XOR obfuscated with key 0x5A)
        char util1[] = {0x3d, 0x3e, 0x38, 0};  // "gdb" ^ 0x5A
        char util2[] = {0x36, 0x36, 0x3e, 0x38, 0};  // "lldb" ^ 0x5A
        char util3[] = {0x29, 0x2e, 0x28, 0x3b, 0x39, 0x3f, 0};  // "strace" ^ 0x5A
        char util4[] = {0x36, 0x2e, 0x28, 0x3b, 0x39, 0x3f, 0};  // "ltrace" ^ 0x5A
        
        for (int i = 0; util1[i]; i++) util1[i] ^= 0x5A;
        for (int i = 0; util2[i]; i++) util2[i] ^= 0x5A;
        for (int i = 0; util3[i]; i++) util3[i] ^= 0x5A;
        for (int i = 0; util4[i]; i++) util4[i] ^= 0x5A;
        
        if (strstr(cmdline, util1) || strstr(cmdline, util2) || 
            strstr(cmdline, util3) || strstr(cmdline, util4)) {
            memory_config = ~memory_config;
            return 0;
        }
    }
    return 0;
}
#else
int check_system_resources() { return 0; }
#endif

#ifdef ENABLE_ANTIDEBUG
// Macro for checking breakpoints on specific memory (no symbol)
#define CHECK_MEMORY_INTEGRITY(addr, len) \
    do { \
        unsigned char* _p = (unsigned char*)(addr); \
        for (size_t _i = 0; _i < (len); _i++) { \
            if (_p[_i] == 0xCC) { \
                /* Manually clear instead of memset to avoid hooks */ \
                for (int _j = 0; _j < sizeof(checksum); _j++) { \
                    ((volatile char*)checksum)[_j] = 0; \
                } \
                return; \
            } \
        } \
    } while(0)
#else
#define CHECK_MEMORY_INTEGRITY(addr, len) do {} while(0)
#endif

#ifdef ENABLE_ANTIDEBUG
// Macro for TracerPid check (no symbol)
#define VALIDATE_RUNTIME_STATE() \
    do { \
        FILE* _f = fopen("/proc/self/status", "r"); \
        if (_f) { \
            char _line[256]; \
            /* Build the search string dynamically to avoid string scanning */ \
            char _search[16]; \
            _search[6] = 'P'; \
            _search[2] = 'a'; \
            _search[9] = ':'; \
            _search[4] = 'e'; \
            _search[1] = 'r'; \
            _search[7] = 'i'; \
            _search[0] = 'T'; \
            _search[5] = 'r'; \
            _search[3] = 'c'; \
            _search[8] = 'd'; \
            _search[10] = '\0'; \
            while (fgets(_line, sizeof(_line), _f)) { \
                if (strncmp(_line, _search, 10) == 0) { \
                    int _pid = atoi(_line + 10); \
                    if (_pid != 0) { \
                        memory_config ^= 0xDEADBEEF; \
                    } \
                    break; \
                } \
            } \
            fclose(_f); \
        } \
    } while(0)
#else
#define VALIDATE_RUNTIME_STATE() do {} while(0)
#endif

static void __attribute__((noinline)) fflush_wrapper(void) {
    fflush(stdout);
    
#ifdef DEBUG_PRINT
    // Print current checksum state for debugging
    printf("DEBUG - Current checksum: %s\n", (char*)checksum);
#endif
    
#ifdef ENABLE_ANTIDEBUG
    // Check for breakpoints at the start of each function
    // Array of function pointers to check
    void* functions[] = {
        (void*)init_memory_manager,
        (void*)setup_heap_allocator,
        (void*)initialize_stack_guard,
        (void*)configure_signal_handlers,
        (void*)setup_exception_handler,
        (void*)initialize_subsystem,
        (void*)setup_network_stack,
        (void*)initialize_thread_pool,
        (void*)configure_memory_layout,
        (void*)finalize_configuration,
        (void*)rotate_order,
        (void*)cleanup_initialization,
        (void*)process_data,
        (void*)print_banner,
        (void*)fflush_wrapper,
        (void*)main
    };
    
    for (int i = 0; i < sizeof(functions)/sizeof(functions[0]); i++) {
        unsigned char* func_start = (unsigned char*)functions[i];
        if (func_start[0] == 0xCC || func_start[0] == 0xEB) {
            // Software breakpoint detected, corrupt the flag
            checksum[0] = 0;
            return;
        }
    }
#endif
}

// Various initialization functions that secretly build the flag
void __attribute__((noinline)) init_memory_manager(void) {
    OVERLAP_V1();
    
    // Anti-debug: ptrace check
    CHECK_PTRACE();
    
    FAKE_FUNCTION_PATTERN();  // Add fake function pattern
    
    // Write "Live" at offset 0 - interleaved with other operations
    volatile int offset = (5 - 5) * 3;  // 0
    letter_B = letter_B ^ 0x01;
    
    if (OPAQUE_PREDICATE_TRUE()) {  // Always true
        checksum[offset] = 'L' ^ letter_B ^ letter_B;  // 'L' at 0
    }
    
    INSERT_JUNK();
}

void __attribute__((noinline)) setup_heap_allocator(void) {
    OVERLAP_V2();
    
    // Anti-debug: timing check
    calculate_checksum();
    
    // Binary integrity check
    CHECK_BINARY_INTEGRITY(); 
    
    // Write some chars from "Live" first
    if (OPAQUE_PREDICATE_FALSE()) {  // Always false
        checksum[1] = 'X';  // Never executed
    } else {
        checksum[1] = 'i' ^ 0x20 ^ 0x20;  // 'i' at 1
    }
    
    OVERLAP_V3();  // Add more confusion
    
    // Write "CTF{" at offset 4 - hidden from static analysis
    if (g_oo) {  // Static analysis thinks this is false
        volatile int base = 2 << 1;  // 4
        letter_B = letter_B + 1;
        checksum[base] = ('C' ^ letter_B) ^ letter_B;  // 'C' at 4
        checksum[base + 1] = 'T';  // 'T' at 5
        checksum[base + 2] = 'F';  // 'F' at 6
        checksum[base + 3] = '{';  // '{' at 7
    }
    
    INSERT_JUNK();
    
    // More from "Live"
    if (!g_oz) {  // Static analysis thinks this is true
        checksum[3] = ('e' + letter_B) - letter_B;  // 'e' at 3
    }
    
    fflush_wrapper();
}

void __attribute__((noinline)) initialize_stack_guard(void) {
    OVERLAP_V4();  // Multiple overlapping jumps
    
    // Calculate metrics for this function
    if (counter((void*)initialize_stack_guard, 100)) {
        checksum[7] = 69;  // Corrupt the flag
        return;
    }
    
    FAKE_FUNCTION_PATTERN();
    
    // Finish "Live" first
    checksum[2] = 'v' ^ 0xFF ^ 0xFF;  // 'v' at 2
    
    OVERLAP_V2();  // Backward jump pattern
    
    // Write "reverse" at offset 8 - hide from static analysis
    if (g_oo) {  // Static analysis thinks this is false
        volatile int idx = (16 >> 1);  // 8
        checksum[idx] = 'r';  // 'r' at 8
        checksum[idx + 1] = 'e';  // 'e' at 9
        checksum[idx + 2] = 'v';  // 'v' at 10
        checksum[idx + 3] = 'e';  // 'e' at 11
        checksum[idx + 4] = 'r';  // 'r' at 12
    }
    
    INSERT_JUNK();
}

void __attribute__((noinline)) configure_signal_handlers(void) {
    OVERLAP_V3();  // Start with overlapping instructions
    
    // Anti-debug: check parent process
    check_system_resources();  // Just call it, don't check return
    
    // Check memory configuration for proper initialization
    if (memory_config != 0x12345678) {
        // Write subtly wrong data instead
        memcpy((void*)(checksum + 15), "_obfuscating", 12);
        return;
    }
    
    // Continue "reverse" from offset 8
    checksum[10] = 'v';  // 'v' at 10
    
    checksum[11] = 'e';  // 'e' at 11
    letter_B ^= 0x02;
    
    // Start "_engineering" at offset 15
    volatile int start = 0xF;  // 15 in hex
    checksum[start] = '_';  // '_' at 15
    
    checksum[12] = 'r';  // 'r' at 12
    
    checksum[start + 1] = 'e';  // 'e' at 16
    fflush_wrapper();
}

void __attribute__((noinline)) setup_exception_handler(void) {
    // Anti-debug: Another timing check
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    // Use time as a simple anti-debug check
    if ((tv.tv_usec & 0xFF) == 0xCC) {  // Unlikely unless breakpoint
        return;
    }
    
    // Continue "reverse" and "_engineering"
    checksum[13] = 's';  // 's' at 13
    
    checksum[17] = 'n';  // 'n' at 17
    
    checksum[14] = 'e';  // 'e' at 14
    
    checksum[18] = 'g';  // 'g' at 18
    letter_B = letter_B + 1;
    
    // Start "_the" at offset 27
    volatile int pos = (54 >> 1);  // 27
    checksum[pos] = '_';  // '_' at 27
}

void __attribute__((noinline)) initialize_subsystem(void) {
    // Continue filling in characters
    checksum[19] = 'i';  // 'i' at 19
    checksum[28] = 't';  // 't' at 28
    checksum[20] = 'n';  // 'n' at 20
    checksum[29] = 'h';  // 'h' at 29
    checksum[21] = 'e';  // 'e' at 21
    letter_B ^= 0x04;
    
    // Write "_trick" at offset 31
    volatile uint32_t offset = 0x1F;  // 31
    const char* fragment = "_trick";
    
    // Verify memory layout is still valid
    if (memory_config != 0x12345678) {
        fragment = "_broke";  // Different length, will mess up the flag
    }
    
    checksum[offset] = fragment[0];  // '_' at 31
    fflush_wrapper();
}

void __attribute__((noinline)) setup_network_stack(void) {
    // Continue filling in
    checksum[22] = 'e';  // 'e' at 22
    checksum[30] = 'e';  // 'e' at 30
    checksum[23] = 'r';  // 'r' at 23
    checksum[32] = 't';  // 't' at 32
    checksum[24] = 'i';  // 'i' at 24
    letter_B = letter_B - 1;
    checksum[33] = 'r';  // 'r' at 33
}

void __attribute__((noinline)) initialize_thread_pool(void) {
    // Continue filling in
    checksum[25] = 'n';  // 'n' at 25
    checksum[34] = 'i';  // 'i' at 34
    checksum[26] = 'g';  // 'g' at 26
    checksum[35] = 'c';  // 'c' at 35
    checksum[36] = 'k';  // 'k' at 36
    letter_B ^= 0x08;
    
    // Write more characters
    checksum[37] = 's';  // 's' at 37
    fflush_wrapper();
}

void __attribute__((noinline)) configure_memory_layout(void) {
    // Continue filling in
    checksum[38] = 't';  // 't' at 38
    checksum[39] = 'e';  // 'e' at 39
    checksum[40] = 'r';  // 'r' at 40
    checksum[41] = '_';  // '_' at 41
    checksum[42] = 'a';  // 'a' at 42
    letter_B = letter_B + 2;
    checksum[43] = 'w';  // 'w' at 43
    
    // Write "ay}" at offset 44
    volatile int pos = (88 >> 1);  // 44
    checksum[pos] = 'a';  // 'a' at 44
    checksum[pos + 1] = 'y';  // 'y' at 45
    checksum[pos + 2] = '}';  // '}' at 46
    fflush_wrapper();
}

void __attribute__((noinline)) finalize_configuration(void) {
    checksum[47] = '\0';
}

void __attribute__((noinline)) rotate_order(void) {
#ifdef DEBUG_PRINT
    printf("DEBUG - Assembled: %s\n", (char*)checksum);
#endif
#ifdef TEST_FLAG
    printf("TEST FLAG: %s\n", (char*)checksum);
#endif
}

void __attribute__((noinline)) cleanup_initialization(void) {
    // Manually clear instead of memset to avoid hooks
    for (int i = 0; i < sizeof(checksum); i++) {
        ((volatile char*)checksum)[i] = 0;
    }
}

void __attribute__((noinline)) process_data(void) {
    // Process system data for optimization
    char message[64] = "Nothing to see here, move along...";
    
    (void)message;  // Suppress unused variable warning
}

void __attribute__((noinline)) print_banner(void) {
    printf("=== Loki's Challenge ===\n");
    printf("The trickster has hidden something...\n");
    printf("Can you find what was concealed?\n\n");
}

void __attribute__((noinline)) halt(void) {
    // This function does nothing but its name will be changed to "exit"
    // in post-processing to confuse analysis tools
    volatile int dummy = 42;
    dummy = dummy * 2;
    return;
}

// Function to make the read-only variables writable
// This runs before main via constructor attribute
__attribute__((constructor)) void __attribute__((noinline)) init_globals(void) {
#ifdef TEST_FLAG
    printf("DEBUG: init_globals() called\n");
#endif
    // Get the page-aligned address of our read-only variables
    void* page_start = (void*)((uintptr_t)&g_oz & ~(getpagesize() - 1));
    size_t page_size = getpagesize();
    
    // Calculate how many pages we need to cover both variables
    void* end_addr = (void*)(&g_oo + 1);
    size_t total_size = (char*)end_addr - (char*)page_start;
    size_t pages_needed = (total_size + page_size - 1) / page_size;
    
    // Make the pages writable
    if (mprotect(page_start, pages_needed * page_size, PROT_READ | PROT_WRITE) == 0) {
        // Cast away const and set the values
        *(int*)&g_oo = 1;
        // g_oz remains 0
#ifdef TEST_FLAG
        printf("DEBUG: Successfully set g_oo = 1\n");
#endif
        
        // Optionally make it read-only again to hide our tracks
        // mprotect(page_start, pages_needed * page_size, PROT_READ);
    }
#ifdef TEST_FLAG
    else {
        printf("DEBUG: mprotect failed!\n");
    }
#endif
}

int main(int argc, char** argv) {
    //OVERLAP_V3();  // Custom pattern with backward jump at entry
    
#ifdef TEST_FLAG
    printf("DEBUG: g_oz = %d, g_oo = %d\n", g_oz, g_oo);
#endif
    
    print_banner();
    
    printf("Initializing subsystems...\n");
    
    // Check binary integrity early
    CHECK_BINARY_INTEGRITY();
    
    FAKE_FUNCTION_PATTERN();
    
    // Call halt early - will be renamed to exit to confuse tools
    halt();
    
    // Validate runtime state
    VALIDATE_RUNTIME_STATE();
    
    OVERLAP_V1();  // Jump over fake instructions
    
    // Call initialization functions that secretly build the flag
    init_memory_manager();
    
    if (OPAQUE_PREDICATE_TRUE()) {
        setup_heap_allocator();
    }
    
    OVERLAP_V4();  // Fake function prologue
    
    initialize_stack_guard();
    configure_signal_handlers();
    
    INSERT_JUNK();
    
    setup_exception_handler();
    initialize_subsystem();
    if (!OPAQUE_PREDICATE_FALSE()) {
        setup_network_stack();
    }
    
    OVERLAP_V1();  // Mix different patterns
    
    initialize_thread_pool();
    configure_memory_layout();
    finalize_configuration();
    
    printf("Verifying system integrity...\n");
    
    FAKE_FUNCTION_PATTERN();
    
    rotate_order();
    process_data();
    printf("Reticulating splines...\n");
    cleanup_initialization();
    
    OVERLAP_V2();  // Add final confusion
    
    printf("\nProgram completed.\n");
    printf("Did you find what you were looking for?\n");
    
    return 0;
}

// Decoy functions that are never called but contain distractor strings
void __attribute__((noinline)) reset_memory_manager(void) {
    OVERLAP_V1();
    
    // Write misleading data to the same checksum buffer
    checksum[0] = 'D';
    checksum[1] = 'e';
    checksum[2] = 'a';
    checksum[3] = 'd';
    
    INSERT_JUNK();
    
    checksum[4] = 'C';
    checksum[5] = 'T';
    checksum[6] = 'F';
    checksum[7] = '{';
    
    // Write "debugging" at offset 8
    checksum[8] = 'd';
    checksum[9] = 'e';
    checksum[10] = 'b';
    checksum[11] = 'u';
    checksum[12] = 'g';
    checksum[13] = 'g';
    checksum[14] = 'i';
    checksum[15] = 'n';
    checksum[16] = 'g';
    
    fflush_wrapper();
}

void __attribute__((noinline)) setup_heap_controller(void) {
    OVERLAP_V2();
    
    // Write different misleading parts
    volatile int offset = 15;
    checksum[offset] = '_';
    checksum[offset + 1] = 's';
    checksum[offset + 2] = 't';
    checksum[offset + 3] = 'a';
    checksum[offset + 4] = 't';
    checksum[offset + 5] = 'i';
    checksum[offset + 6] = 'c';
    
    FAKE_FUNCTION_PATTERN();
    
    // "_analysis" at offset 22
    checksum[22] = '_';
    checksum[23] = 'a';
    checksum[24] = 'n';
    checksum[25] = 'a';
    checksum[26] = 'l';
    checksum[27] = 'y';
    checksum[28] = 's';
    checksum[29] = 'i';
    checksum[30] = 's';
}

void __attribute__((noinline)) initialize_stack_protector(void) {
    OVERLAP_V3();
    
    // Overwrite with "Fake"
    checksum[0] = 'F';
    checksum[1] = 'a';
    checksum[2] = 'k';
    checksum[3] = 'e';
    
    INSERT_JUNK();
    
    // Write "_breaking" at offset 31
    volatile int pos = 31;
    checksum[pos] = '_';
    checksum[pos + 1] = 'b';
    checksum[pos + 2] = 'r';
    checksum[pos + 3] = 'e';
    checksum[pos + 4] = 'a';
    checksum[pos + 5] = 'k';
    checksum[pos + 6] = 'i';
    checksum[pos + 7] = 'n';
    checksum[pos + 8] = 'g';
    
    fflush_wrapper();
}

void __attribute__((noinline)) configure_signal_dispatcher(void) {
    OVERLAP_V4();
    
    // Write "Test" instead of "Live"
    checksum[0] = 'T';
    checksum[1] = 'e';
    checksum[2] = 's';
    checksum[3] = 't';
    
    FAKE_FUNCTION_PATTERN();
    
    // "_patched_" at offset 31
    checksum[31] = '_';
    checksum[32] = 'p';
    checksum[33] = 'a';
    checksum[34] = 't';
    checksum[35] = 'c';
    checksum[36] = 'h';
    checksum[37] = 'e';
    checksum[38] = 'd';
    checksum[39] = '_';
    
    // "here}" at offset 40
    checksum[40] = 'h';
    checksum[41] = 'e';
    checksum[42] = 'r';
    checksum[43] = 'e';
    checksum[44] = '}';
    checksum[45] = '\0';
}

void __attribute__((noinline)) reset_configuration(void) {
    OVERLAP_V1();
    
    // Another variant
    checksum[8] = 'f';
    checksum[9] = 'o';
    checksum[10] = 'r';
    checksum[11] = 'w';
    checksum[12] = 'a';
    checksum[13] = 'r';
    checksum[14] = 'd';
    
    INSERT_JUNK();
    
    // "_cracking" at offset 15
    checksum[15] = '_';
    checksum[16] = 'c';
    checksum[17] = 'r';
    checksum[18] = 'a';
    checksum[19] = 'c';
    checksum[20] = 'k';
    checksum[21] = 'i';
    checksum[22] = 'n';
    checksum[23] = 'g';
    
    // Clear some later parts
    checksum[46] = '\0';
}
