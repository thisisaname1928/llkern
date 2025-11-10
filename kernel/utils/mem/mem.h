#include <stddef.h>
#include <stdint.h>

#ifndef MEM_H
#define MEM_H

void memcpy(void *dest, const void *src, uint32_t n);
void memmove(void *dest, const void *src, size_t n);
int memcmp(const void *ptr1, const void *ptr2, size_t num);
uint32_t align(uint32_t addr, uint32_t boundary);

#endif