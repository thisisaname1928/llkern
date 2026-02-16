#include <stdint.h>
#ifndef SLAB_H
#define SLAB_H
#define SLAB_SZ 32

typedef struct {
  uint32_t biggestSlabSz;
  uint32_t blockSz;
  struct SlabBlockHeader *prev;
  struct SlabBlockHeader *next;
} SlabBlockHeader;

void initNewSlabBLock(void *beginBlock, uint32_t sz);
void *slabAlloc(SlabBlockHeader *hdr, uint32_t memSz);

#endif