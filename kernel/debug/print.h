#include <stdint.h>
#ifndef PRINT_H
#define PRINT_H
void putchar(int c);
void newline();
void scroll();
void printStr(const char *str);
void printUint(uint32_t n);
void printHex(uint32_t n);
void kprintf(const char *str, ...);
#endif