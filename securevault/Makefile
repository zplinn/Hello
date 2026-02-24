# ──────────────────────────────────────────────────────────
#  SecureVault — CTF PWN Challenge (hard)
#
#  Target   : x86-64 Linux (glibc)
#  Compile  : make
#  Clean    : make clean
#
#  Binary protections (Linux target)
#  ─────────────────────────────────
#    NX            : ENABLED   (no shellcode on stack/heap)
#    Partial RELRO : ENABLED   (GOT is writable)
#    PIE           : DISABLED  (binary base is fixed)
#    Stack canary  : DISABLED  (focus is on heap, not stack)
#    Fortify       : DISABLED
# ──────────────────────────────────────────────────────────

CC       := gcc
TARGET   := securevault
SRC      := vault.c

CFLAGS   := -Wall -Wextra -Wno-unused-result -O0 \
            -fno-stack-protector \
            -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0

UNAME_S  := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
  CFLAGS  += -no-pie
  LDFLAGS := -no-pie -Wl,-z,relro
else
  # macOS / other — build without ELF-specific linker flags
  LDFLAGS :=
endif

# ── build rules ──────────────────────────────────────────

.PHONY: all clean checksec

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	@echo ""
	@echo "[+] Built $(TARGET)"
ifeq ($(UNAME_S),Linux)
	@echo "    Protections: NX | Partial RELRO | No PIE | No canary"
else
	@echo "    NOTE: Built on $(UNAME_S). Deploy on Linux for intended protections."
endif
	@echo "    Run:  ./$(TARGET)"
	@echo ""

clean:
	rm -f $(TARGET)

checksec: $(TARGET)
	@command -v checksec >/dev/null 2>&1 && checksec --file=$(TARGET) || \
	 python3 -c "from pwn import *; print(ELF('./$(TARGET)').checksec())" 2>/dev/null || \
	 echo "Install checksec or pwntools to inspect binary protections."
