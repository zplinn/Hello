/*
 *  SecureVault  —  hardened data-storage service  (v3.1)
 *
 *  "Enterprise-grade security for all your sensitive data."
 *
 *  Challenge category : PWN  (hard)
 *  Remote flag        : /flag.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>

#define MAX_VAULTS   8
#define NAME_LEN     32
#define MAX_ALLOC    0x10000   /* 64 KiB ceiling */

/* ---------- data model ---------- */

typedef struct {
    char     name[NAME_LEN];
    char    *data;
    uint32_t n_entries;
    uint32_t entry_sz;
    uint32_t total_sz;
    int      in_use;
} vault_t;

static vault_t vaults[MAX_VAULTS];

/* ---------- helpers ---------- */

static void timeout_handler(int sig) {
    (void)sig;
    write(STDERR_FILENO, "\n[!] timeout\n", 13);
    _exit(1);
}

static void setup(void) {
    setvbuf(stdin,  NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    signal(SIGALRM, timeout_handler);
    alarm(120);
}

static int get_int(const char *prompt) {
    int v = 0;
    printf("%s", prompt);
    if (scanf("%d", &v) != 1) {
        puts("[-] input error");
        exit(1);
    }
    return v;
}

static uint32_t get_uint(const char *prompt) {
    uint32_t v = 0;
    printf("%s", prompt);
    if (scanf("%u", &v) != 1) {
        puts("[-] input error");
        exit(1);
    }
    return v;
}

static void read_name(char *dst, size_t max) {
    getchar();
    size_t n = 0;
    int c;
    while (n < max - 1 && (c = getchar()) != EOF && c != '\n')
        dst[n++] = (char)c;
    dst[n] = '\0';
}

/* ---------- vault operations ---------- */

static void do_create(void) {
    int slot = -1;
    for (int i = 0; i < MAX_VAULTS; i++) {
        if (!vaults[i].in_use) { slot = i; break; }
    }
    if (slot < 0) {
        puts("[-] All vault slots occupied.");
        return;
    }

    printf("  Name: ");
    read_name(vaults[slot].name, NAME_LEN);

    uint32_t n = get_uint("  Number of entries : ");
    uint32_t s = get_uint("  Bytes per entry   : ");

    if (n == 0 || s == 0) {
        puts("[-] Dimensions must be non-zero.");
        return;
    }

    uint32_t total = n * s;

    if (total == 0) {
        puts("[-] Invalid configuration.");
        return;
    }

    if (total > MAX_ALLOC) {
        printf("[-] Requested %u bytes — limit is %u.\n", total, MAX_ALLOC);
        return;
    }

    void *buf = calloc(1, total);
    if (!buf) {
        puts("[-] Allocation failure.");
        return;
    }

    vaults[slot].data      = buf;
    vaults[slot].n_entries = n;
    vaults[slot].entry_sz  = s;
    vaults[slot].total_sz  = total;
    vaults[slot].in_use    = 1;

    printf("[+] Vault #%d \"%s\" — %u slots of %u B  (total %u B)\n",
           slot, vaults[slot].name, n, s, total);
}

static void do_write(void) {
    int slot = get_int("  Vault #: ");

    if (slot < 0 || slot >= MAX_VAULTS || !vaults[slot].in_use) {
        puts("[-] Invalid vault.");
        return;
    }

    uint32_t idx = get_uint("  Entry index: ");
    if (idx >= vaults[slot].n_entries) {
        puts("[-] Index out of range.");
        return;
    }

    size_t   off = (size_t)idx * vaults[slot].entry_sz;
    uint32_t len = vaults[slot].entry_sz;

    printf("  Data (%u bytes): ", len);
    getchar();
    read(STDIN_FILENO, vaults[slot].data + off, len);

    puts("[+] Entry written.");
}

static void do_read(void) {
    int slot = get_int("  Vault #: ");

    if (slot < 0 || slot >= MAX_VAULTS || !vaults[slot].in_use) {
        puts("[-] Invalid vault.");
        return;
    }

    uint32_t idx = get_uint("  Entry index: ");
    if (idx >= vaults[slot].n_entries) {
        puts("[-] Index out of range.");
        return;
    }

    size_t   off = (size_t)idx * vaults[slot].entry_sz;
    uint32_t len = vaults[slot].entry_sz;

    printf("[*] ");
    write(STDOUT_FILENO, vaults[slot].data + off, len);
    putchar('\n');
}

static void do_destroy(void) {
    int slot = get_int("  Vault #: ");

    if (slot < 0 || slot >= MAX_VAULTS || !vaults[slot].in_use) {
        puts("[-] Invalid vault.");
        return;
    }

    free(vaults[slot].data);
    memset(&vaults[slot], 0, sizeof(vault_t));

    puts("[+] Vault destroyed.");
}

static void do_info(void) {
    int any = 0;
    puts("  ┌─────────────────────────────────────┐");
    for (int i = 0; i < MAX_VAULTS; i++) {
        if (vaults[i].in_use) {
            printf("  │ #%d  %-10s  %5u x %-5u  (%5u) │\n",
                   i, vaults[i].name,
                   vaults[i].n_entries, vaults[i].entry_sz,
                   vaults[i].total_sz);
            any = 1;
        }
    }
    if (!any)
        puts("  │         (no active vaults)         │");
    puts("  └─────────────────────────────────────┘");
}

/* ---------- main loop ---------- */

int main(void) {
    setup();

    puts("╔═══════════════════════════════════╗");
    puts("║   SecureVault Storage Service     ║");
    puts("║        v3.1  (hardened)           ║");
    puts("╚═══════════════════════════════════╝");
    putchar('\n');

    for (;;) {
        puts("[1] Create   [2] Write   [3] Read");
        puts("[4] Destroy  [5] Info    [6] Exit");

        switch (get_int("> ")) {
            case 1: do_create();  break;
            case 2: do_write();   break;
            case 3: do_read();    break;
            case 4: do_destroy(); break;
            case 5: do_info();    break;
            case 6:
                puts("[*] Goodbye.");
                return 0;
            default:
                puts("[-] Unknown option.");
        }
        putchar('\n');
    }
}
