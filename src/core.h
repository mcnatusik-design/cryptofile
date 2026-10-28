#ifndef CORE_H
#define CORE_H

#include <stddef.h>

int cf_derive_key(unsigned char *key, const char *password, const unsigned char *salt);
int cf_encrypt_file(const char *in_path, const char *out_path, const char *password);
int cf_decrypt_file(const char *in_path, const char *out_path, const char *password);
void cf_gen_key(char *hex_out, size_t hex_len);

#endif
