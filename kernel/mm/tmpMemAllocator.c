#include "mm.h"
#include <stddef.h>
#include <stdint.h>

// this for early kernel memory allocation, use by using stack as free memory;
// these buffer will be destroyed after the root function has gone

TmpMemAllocator *curTmpMemAllocator;

void initTmpMemAllocator(TmpMemAllocator *allocator, void *buffer,
                         uint32_t memSz) {
  allocator->beginBlock = buffer;
  allocator->curPtr = buffer;
  allocator->memSz = memSz;
}

void *tmpMemAlloc(TmpMemAllocator *allocator, uint32_t memSz) {
  if (allocator == NULL) {
    return NULL;
  }

  if (allocator->memSz -
          ((uint32_t)(allocator->curPtr) - (uint32_t)(allocator->beginBlock)) >=
      memSz) {
    void *freeMem = allocator->curPtr;
    allocator->curPtr = (char *)allocator->curPtr + memSz;

    return freeMem;
  } else
    return NULL;
}