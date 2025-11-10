#include <stdint.h>
#ifndef PAGEALLOCATOR_H
#define PAGEALLOCATOR_H

int initPageAllocator();
void *allocPages(int32_t n);
void freePages(void *ptr);

extern int32_t *freePagesList;

#endif