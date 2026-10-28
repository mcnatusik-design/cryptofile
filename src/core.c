#include "core.h"
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cf_derive_key(unsigned char *key, const char *password, const unsigned char *salt) {
    if (crypto_pwhash(key, crypto_secretstream_xchacha20poly1305_KEYBYTES,
                      password, strlen(password), salt,
                      crypto_pwhash_OPSLIMIT_INTERACTIVE,
                      crypto_pwhash_MEMLIMIT_INTERACTIVE,
                      crypto_pwhash_ALG_DEFAULT) != 0) {
        return -1;
    }
    return 0;
}

int cf_encrypt_file(const char *in_path, const char *out_path, const char *password) {
    FILE *fin = fopen(in_path, "rb");
    if (!fin) return -1;
    FILE *fout = fopen(out_path, "wb");
    if (!fout) { fclose(fin); return -1; }

    unsigned char salt[crypto_pwhash_SALTBYTES];
    randombytes_buf(salt, sizeof(salt));
    fwrite(salt, 1, sizeof(salt), fout);

    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    if (cf_derive_key(key, password, salt) != 0) {
        fclose(fin); fclose(fout); return -2;
    }

    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, key);
    fwrite(header, 1, sizeof(header), fout);

    unsigned char in_buf[4096];
    unsigned char out_buf[4096 + crypto_secretstream_xchacha20poly1305_ABYTES];
    size_t read_bytes;
    int ret = 0;

    while ((read_bytes = fread(in_buf, 1, sizeof(in_buf), fin)) > 0) {
        unsigned char tag = feof(fin) ? crypto_secretstream_xchacha20poly1305_TAG_FINAL
                                      : crypto_secretstream_xchacha20poly1305_TAG_MESSAGE;
        unsigned long long out_len;
        crypto_secretstream_xchacha20poly1305_push(&state, out_buf, &out_len, in_buf, read_bytes, NULL, 0, tag);
        fwrite(out_buf, 1, (size_t)out_len, fout);
    }

    sodium_memzero(key, sizeof(key));
    fclose(fin); fclose(fout);
    return ret;
}

int cf_decrypt_file(const char *in_path, const char *out_path, const char *password) {
    FILE *fin = fopen(in_path, "rb");
    if (!fin) return -1;
    FILE *fout = fopen(out_path, "wb");
    if (!fout) { fclose(fin); return -1; }

    unsigned char salt[crypto_pwhash_SALTBYTES];
    if (fread(salt, 1, sizeof(salt), fin) != sizeof(salt)) { fclose(fin); fclose(fout); return -3; }

    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    if (cf_derive_key(key, password, salt) != 0) {
        fclose(fin); fclose(fout); return -2;
    }

    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    if (fread(header, 1, sizeof(header), fin) != sizeof(header)) { fclose(fin); fclose(fout); return -3; }

    crypto_secretstream_xchacha20poly1305_state state;
    if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key) != 0) {
        fclose(fin); fclose(fout); return -4;
    }

    unsigned char in_buf[4096 + crypto_secretstream_xchacha20poly1305_ABYTES];
    unsigned char out_buf[4096];
    size_t read_bytes;
    int ret = 0;

    while ((read_bytes = fread(in_buf, 1, sizeof(in_buf), fin)) > 0) {
        unsigned char tag;
        unsigned long long out_len;
        if (crypto_secretstream_xchacha20poly1305_pull(&state, out_buf, &out_len, &tag, in_buf, read_bytes, NULL, 0) != 0) {
            ret = -5;
            break;
        }
        fwrite(out_buf, 1, (size_t)out_len, fout);
        if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL) break;
    }

    sodium_memzero(key, sizeof(key));
    fclose(fin); fclose(fout);
    return ret;
}

void cf_gen_key(char *hex_out, size_t hex_len) {
    unsigned char key[crypto_secretstream_xchacha20poly1305_KEYBYTES];
    crypto_secretstream_xchacha20poly1305_keygen(key);
    sodium_bin2hex(hex_out, hex_len, key, sizeof(key));
}
