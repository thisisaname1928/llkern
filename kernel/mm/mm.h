#include <stdint.h>
#ifndef PAGEALLOCATOR_H
#define PAGEALLOCATOR_H

typedef struct {
  void *beginBlock;
  void *curPtr;
  uint32_t memSz;
} TmpMemAllocator;

void *tmpMemAlloc(TmpMemAllocator *allocator, uint32_t memSz);
void initTmpMemAllocator(TmpMemAllocator *allocator, void *buffer,
                         uint32_t memSz);

// use as reference to the root func allocator, example: initKernel() in main.c
extern TmpMemAllocator *curTmpMemAllocator;

int initPageAllocator();
void *allocPages(int32_t n);
void freePages(void *ptr);

extern int32_t *freePagesList;

#endif