#include "slab.h"
#include <debug/print.h>
#include <stddef.h>
#include <stdint.h>
#include <utils/math.h>

// use int16_t as slabHdr, > 0 -> free, < 0 -> not free
typedef int32_t SlabHdr;

void initNewSlabBLock(void *beginBlock, uint32_t sz) {
  SlabBlockHeader *hdr = beginBlock;
  hdr->blockSz = sz;

  // add first blockHdr
  SlabHdr *slabHdr = (SlabHdr *)((char *)beginBlock + sizeof(SlabBlockHeader));
  *slabHdr = (sz - sizeof(SlabBlockHeader)) / SLAB_SZ;

  kprintf("ok = %u\n", *slabHdr);

  // add a sign to stop at the end of block
  if ((sz - sizeof(SlabBlockHeader)) % 32 >= 2) {
    SlabHdr *ptr = (SlabHdr *)(hdr + sz);
    *ptr = 0;
  }
}

void *slabAlloc(SlabBlockHeader *hdr, uint32_t memSz) {
  if (memSz == sizeof(SlabHdr)) // cause 0 % 4
    memSz++;

  while ((memSz - sizeof(SlabHdr)) % SLAB_SZ != 0) {
    memSz++;
  }

  int numberOfSlab = memSz / SLAB_SZ;
  kprintf("alloc slab = %u\n", numberOfSlab);

  SlabHdr *slabHdr = (SlabHdr *)((char *)hdr + sizeof(SlabBlockHeader));

  // find suitable block
  while (*slabHdr != 0) {
    kprintf("slabSz = %u\n", abs(*slabHdr));
    // there is no more slab to find
    if ((uint32_t)(slabHdr) + SLAB_SZ > (uint32_t)(hdr) + hdr->blockSz) {
      return NULL;
    }

    // ok find it=))
    if (*slabHdr > 0 && *slabHdr > numberOfSlab) {
      int32_t curSlabSz = *slabHdr;
      SlabHdr *curHdr = slabHdr;
      *curHdr = -numberOfSlab;
      slabHdr = (SlabHdr *)((char *)(slabHdr) + numberOfSlab * SLAB_SZ);
      *slabHdr = curSlabSz - numberOfSlab;

      kprintf("ok divide slab a = %u, b = %u\n", curHdr, slabHdr);

      return (char *)curHdr + sizeof(SlabHdr);
    } else {
      slabHdr = (SlabHdr *)((char *)(slabHdr) + abs(*slabHdr) * SLAB_SZ);
    }
  }

  return NULL;
}