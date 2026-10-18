#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sezar.h"

static void print_usage(FILE* out) {
    fprintf(out,
        "The New Sezar Algorithm\n"
        "-----------------------\n"
        "A toy cipher that multiplies the encoded digit-string by a shared key.\n\n"
        "Usage:\n"
        "  sezar encrypt \"<text>\" <key>     Encrypt a message\n"
        "  sezar decrypt \"<cipher>\" <key>   Decrypt a ciphertext\n\n"
        "Key range: 2 to 9,999,999  (0 and 1 are forbidden)\n\n"
        "Encoding:\n"
        "  a-z -> 01..26    space -> 27    everything else is ignored\n\n"
        "WARNING: This is a fun puzzle cipher, NOT real encryption.\n"
        "         The entire key space can be brute-forced in seconds.\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(stderr);
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(stdout);
        return 0;
    }

    if (strcmp(argv[1], "encrypt") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: 'encrypt' requires <text> and <key>\n");
            fprintf(stderr, "  Example: sezar encrypt \"hello world\" 4781\n");
            return 1;
        }
        long long key = atoll(argv[3]);
        if (!sezar_key_valid(key)) {
            fprintf(stderr, "Error: key must be between 2 and 9,999,999 (got %lld)\n", key);
            return 1;
        }
        char* result = sezar_encrypt(argv[2], key);
        if (!result) {
            fprintf(stderr, "Error: nothing to encrypt (input had no letters or spaces)\n");
            return 1;
        }
        printf("%s\n", result);
        free(result);
        return 0;
    }

    if (strcmp(argv[1], "decrypt") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Error: 'decrypt' requires <ciphertext> and <key>\n");
            fprintf(stderr, "  Example: sezar decrypt 3849284530216938881336324 4781\n");
            return 1;
        }
        long long key = atoll(argv[3]);
        if (!sezar_key_valid(key)) {
            fprintf(stderr, "Error: key must be between 2 and 9,999,999 (got %lld)\n", key);
            return 1;
        }
        char* result = sezar_decrypt(argv[2], key);
        if (!result) {
            fprintf(stderr, "Error: decryption failed (wrong key, or ciphertext is corrupted)\n");
            return 1;
        }
        printf("%s\n", result);
        free(result);
        return 0;
    }

    fprintf(stderr, "Error: unknown command '%s'\n\n", argv[1]);
    print_usage(stderr);
    return 1;
}
