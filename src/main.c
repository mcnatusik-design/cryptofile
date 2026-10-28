#include "core.h"
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef APP_VERSION
#define APP_VERSION "0.0.0"
#endif

void print_help(const char *prog) {
    printf("Usage: %s <command> [options]\n", prog);
    printf("Commands:\n");
    printf("  encrypt <in> <out> -p <password>  Encrypt file\n");
    printf("  decrypt <in> <out> -p <password>  Decrypt file\n");
    printf("  genkey                            Generate a random key (hex)\n");
    printf("  --version                         Show version\n");
    printf("  --help                            Show this help\n");
}

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) { 
        fprintf(stderr, "Failed to init libsodium\n"); 
        return 1; 
    }
    
    if (argc < 2) { 
        print_help(argv[0]); 
        return 1; 
    }

    if (strcmp(argv[1], "--version") == 0) { 
        printf("cryptofile %s\n", APP_VERSION); 
        return 0; 
    }
    
    if (strcmp(argv[1], "--help") == 0) { 
        print_help(argv[0]); 
        return 0; 
    }

    if (strcmp(argv[1], "genkey") == 0) {
        char hex[65];
        cf_gen_key(hex, sizeof(hex));
        printf("%s\n", hex);
        return 0;
    }

    if (strcmp(argv[1], "encrypt") == 0 || strcmp(argv[1], "decrypt") == 0) {
        if (argc < 5) { 
            fprintf(stderr, "Missing arguments. Try --help\n"); 
            return 1; 
        }
        
        const char *pass = NULL;
        for (int i = 2; i < argc - 1; i++) {
            if (strcmp(argv[i], "-p") == 0) {
                pass = argv[i+1];
            }
        }
        
        if (!pass) { 
            fprintf(stderr, "Password required (-p)\n"); 
            return 1; 
        }

        int rc;
        if (strcmp(argv[1], "encrypt") == 0) {
            rc = cf_encrypt_file(argv[2], argv[3], pass);
        } else {
            rc = cf_decrypt_file(argv[2], argv[3], pass);
        }

        if (rc != 0) { 
            fprintf(stderr, "Operation failed with code %d\n", rc); 
            return 1; 
        }
        return 0;
    }

    fprintf(stderr, "Unknown command: %s\n", argv[1]);
    return 1;
}
