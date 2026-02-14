
#include "multiboot2/multiboot2.h"
#include <debug/print.h>
#include <stdint.h>
#include <utils/mem/mem.h>

uint32_t findNearestPowerOfTwo(uint32_t n) {
  while (n > 0) {
    if (!isPowerOfTwo(n))
      n--;
    else
      break;
  }

  return n;
}

void initBuddyAllocator() {
  // detect free blocks
  multiboot2MemoryMapEntry *entry = memmap->entries;

  while ((uint32_t)entry < (uint32_t)memmap + memmap->size) {
    if (entry->type == MULTIBOOT2_MEMORY_AVAILABLE) {
      uint32_t curBlockSz = entry->length / 4096;

      while (curBlockSz > 0) {
        printStr("Divide into: ");

        uint32_t nb = findNearestPowerOfTwo(curBlockSz);
        printUint(nb);
        newline();

        if (nb == 0) {
          break;
        }

        curBlockSz -= nb;
      }
    }

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }
}