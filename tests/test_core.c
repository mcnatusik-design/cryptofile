#include "core.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sodium.h>

// Вспомогательная функция для создания тестового файла
void create_dummy_file(const char *path, const char *content) {
    FILE *f = fopen(path, "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
}

// Вспомогательная функция для чтения файла
long get_file_size(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    return size;
}

// ТЕСТ 1: Базовая проверка шифрования и расшифровки
void test_encrypt_decrypt_basic() {
    const char *in = "test_in.txt", *enc = "test.enc", *dec = "test_dec.txt";
    const char *pass = "secret123";
    create_dummy_file(in, "Hello, world!");
    
    assert(cf_encrypt_file(in, enc, pass) == 0);
    assert(cf_decrypt_file(enc, dec, pass) == 0);
    
    FILE *f1 = fopen(in, "r"); FILE *f2 = fopen(dec, "r");
    char buf1[64], buf2[64];
    fgets(buf1, sizeof(buf1), f1); fgets(buf2, sizeof(buf2), f2);
    assert(strcmp(buf1, buf2) == 0);
    fclose(f1); fclose(f2);
    printf("Test 1: Basic encrypt/decrypt - PASSED\n");
}

// ТЕСТ 2: Пустой файл
void test_empty_file() {
    const char *in = "empty_in.txt", *enc = "empty.enc", *dec = "empty_dec.txt";
    const char *pass = "secret123";
    create_dummy_file(in, "");
    
    assert(cf_encrypt_file(in, enc, pass) == 0);
    assert(cf_decrypt_file(enc, dec, pass) == 0);
    assert(get_file_size(dec) == 0);
    printf("Test 2: Empty file - PASSED\n");
}

// ТЕСТ 3: Неверный пароль (расшифровка должна провалиться)
void test_wrong_password() {
    const char *in = "test_in.txt", *enc = "test.enc", *dec = "test_bad.txt";
    create_dummy_file(in, "Secret data");
    
    assert(cf_encrypt_file(in, enc, "correct_pass") == 0);
    // Пытаемся расшифровать неверным паролем
    assert(cf_decrypt_file(enc, dec, "wrong_pass") != 0);
    printf("Test 3: Wrong password - PASSED\n");
}

// ТЕСТ 4: Несуществующий файл
void test_non_existent_file() {
    assert(cf_encrypt_file("no_such_file.txt", "out.enc", "pass") != 0);
    printf("Test 4: Non-existent input file - PASSED\n");
}

// ТЕСТ 5: Проверка деривации ключа (детерминированность)
void test_key_derivation() {
    unsigned char key1[32], key2[32];
    unsigned char salt[16] = {0};
    const char *pass = "my_password";
    
    assert(cf_derive_key(key1, pass, salt) == 0);
    assert(cf_derive_key(key2, pass, salt) == 0);
    assert(memcmp(key1, key2, 32) == 0);
    printf("Test 5: Key derivation consistency - PASSED\n");
}

// ТЕСТ 6: Генерация ключа (длина hex-строки)
void test_gen_key() {
    char hex[65];
    cf_gen_key(hex, sizeof(hex));
    assert(strlen(hex) == 64); // 32 байта = 64 hex-символа
    printf("Test 6: Key generation - PASSED\n");
}

// ТЕСТ 7: Поврежденный шифротекст (аутентификация должна провалиться)
void test_corrupted_data() {
    const char *in = "test_in.txt", *enc = "test.enc", *dec = "test_corrupt.txt";
    create_dummy_file(in, "Data to corrupt");
    cf_encrypt_file(in, enc, "pass");
    
    // Портим один байт в середине файла
    FILE *f = fopen(enc, "r+b");
    fseek(f, 20, SEEK_SET);
    fputc(0xFF, f);
    fclose(f);
    
    assert(cf_decrypt_file(enc, dec, "pass") != 0);
    printf("Test 7: Corrupted data detection - PASSED\n");
}

// ТЕСТ 8: Граничный случай - большой файл (100 КБ)
void test_large_file() {
    const char *in = "large_in.txt", *enc = "large.enc", *dec = "large_dec.txt";
    FILE *f = fopen(in, "w");
    for (int i = 0; i < 100000; i++) fputc('A', f);
    fclose(f);
    
    assert(cf_encrypt_file(in, enc, "pass") == 0);
    assert(cf_decrypt_file(enc, dec, "pass") == 0);
    assert(get_file_size(in) == get_file_size(dec));
    printf("Test 8: Large file (100KB) - PASSED\n");
}

int main() {
    if (sodium_init() < 0) return 1;
    
    test_encrypt_decrypt_basic();
    test_empty_file();
    test_wrong_password();
    test_non_existent_file();
    test_key_derivation();
    test_gen_key();
    test_corrupted_data();
    test_large_file();
    
    printf("All core tests PASSED successfully!\n");
    return 0;
}
