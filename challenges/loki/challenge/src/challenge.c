#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include "flag_hash.h"

#define MAX_ATTEMPTS 5
#define TIMEOUT_SECONDS 30

void win(void) {
    system("/bin/sh");
}

void init(void) {
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

void handle_timeout(int sig) {
    printf("Connection timeout!\n");
    exit(1);
}

int validate_flag(const char* input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input, strlen(input), hash);
    
    char hex_hash[SHA256_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex_hash + (i * 2), "%02x", hash[i]);
    }
    hex_hash[SHA256_DIGEST_LENGTH * 2] = '\0';
    
    return strcmp(hex_hash, FLAG_HASH) == 0;
}

void handle_connection() {
    char buffer[256] = {0};
    int attempts = 0;
    
    // Set connection timeout
    signal(SIGALRM, handle_timeout);
    alarm(TIMEOUT_SECONDS);
    
    printf("=== Loki's Flag Verifier ===\n");
    printf("Submit the flag you extracted from the challenge binary.\n");
    printf("Correct submission grants shell access.\n\n");
    fflush(stdout);
    
    while (attempts < MAX_ATTEMPTS) {
        memset(buffer, 0, sizeof(buffer));
        printf("Enter flag: ");
        fflush(stdout);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        // Remove newline if present
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if (validate_flag(buffer)) {
            printf("\n[+] Correct! The trickster grants you access...\n\n");
            fflush(stdout);
            alarm(0); // Cancel timeout
            
            win();
            exit(0);
        } else {
            attempts++;
            printf("[-] Wrong! %d/%d attempts used.\n", attempts, MAX_ATTEMPTS);
            fflush(stdout);
        }
    }
    
    printf("\nToo many failed attempts. Goodbye!\n");
    fflush(stdout);
    exit(1);
}

int main(int argc, char** argv, char** envp) {
    init();
    
    // Handle the connection directly using stdin/stdout
    handle_connection();
    
    return 0;
}