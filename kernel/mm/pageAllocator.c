#include "bootstrap.h"
#include <debug/print.h>
#include <multiboot2/multiboot2.h>
#include <stdint.h>
#include <utils/math.h>

// index as address (index * 4096), store information about how long the memory
// map is
// msb bit for free or non free pages
int32_t *freePagesList;
multiboot2MemoryMapEntry highestFreeEntry;

// return 1 if success, otherwise return 0
int initPageAllocator() {
  highestFreeEntry.baseAddr = 0;
  highestFreeEntry.length = 0;

  // detect highest free entry
  multiboot2MemoryMapEntry *entry = memmap->entries;
  while ((uint32_t)entry < (uint32_t)memmap + memmap->size) {
    if (highestFreeEntry.baseAddr < entry->baseAddr &&
        entry->type == MULTIBOOT2_MEMORY_AVAILABLE) {
      highestFreeEntry.baseAddr = entry->baseAddr;
      highestFreeEntry.length = entry->length;
    }

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }

  // init freePagesList
  printStr("init pages list...\n");
  freePagesList = (void *)kernelEndAddr; // place at end of the kernel

  // detect free pages
  printStr("detect free pages...\n");
  entry = memmap->entries;
  multiboot2MemoryMapEntry *lastNonFreeEntry = &memmap->entries[0];
  while ((uint32_t)entry < (uint32_t)memmap + memmap->size) {
    if (entry->baseAddr > highestFreeEntry.baseAddr) {
      break;
    }

    // update lastNonFreeEntry
    if (lastNonFreeEntry->type == MULTIBOOT2_MEMORY_AVAILABLE &&
        entry->type != MULTIBOOT2_MEMORY_AVAILABLE) {
      lastNonFreeEntry = entry;
    }

    // add a continous non free block
    if (lastNonFreeEntry->type != MULTIBOOT2_MEMORY_AVAILABLE &&
        entry->type == MULTIBOOT2_MEMORY_AVAILABLE) {
      // non free block
      freePagesList[lastNonFreeEntry->baseAddr / 4096] =
          -((entry->baseAddr - lastNonFreeEntry->baseAddr) / 4096);

      // free block
      freePagesList[entry->baseAddr / 4096] = entry->length / 4096;

      printStr("Non Free Block: ");
      printHex(lastNonFreeEntry->baseAddr);
      printStr(" ");
      printUint((entry->baseAddr - lastNonFreeEntry->baseAddr) / 4096);
      printStr("\nFree block: ");
      printHex(entry->baseAddr);
      printStr(" ");
      printUint((entry->length) / 4096);
      newline();
    }

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }

  // traversal test
  uint32_t baseAddr = 0;
  while (baseAddr <= highestFreeEntry.baseAddr) {
    if (freePagesList[baseAddr / 4096] < 0) {
      baseAddr += abs(freePagesList[baseAddr / 4096]) * 4096;

    } else {
      printStr("FOUND FREE BLOCK!\n");
      printHex(baseAddr);
      printStr(" sz=");
      printUint(freePagesList[baseAddr / 4096] * 4096);
      newline();
      baseAddr += freePagesList[baseAddr / 4096] * 4096;
    }
  }

  return 1;
}

void *simpleAlloc(int32_t n) {
  uint32_t baseAddr = 0;
  while (baseAddr <= highestFreeEntry.baseAddr + highestFreeEntry.length) {

    if (freePagesList[baseAddr / 4096] < 0) {
      baseAddr += abs(freePagesList[baseAddr / 4096]) * 4096;

    } else {
      // found free block
      if (freePagesList[baseAddr / 4096] > n) {
        // divide it into 2 block
        freePagesList[baseAddr / 4096 + n] =
            freePagesList[baseAddr / 4096] - n; // free
        freePagesList[baseAddr / 4096] = -n;    // non free
        printStr("DEB: ");
        printHex(baseAddr);
        newline();

        return (void *)baseAddr;
      } else if (freePagesList[baseAddr / 4096] == n) {
        freePagesList[baseAddr / 4096] = -n;
        return (void *)baseAddr;
      } else {
        baseAddr += freePagesList[baseAddr / 4096] * 4096;
      }
    }
  }

  return 0;
}

void simpleDealloc(void *ptr) {
  uint32_t baseAddr = (uint32_t)ptr;
  uint32_t nPages = freePagesList[baseAddr / 4096];
  freePagesList[baseAddr / 4096] = abs(freePagesList[baseAddr / 4096]);

  // check if next block is free
  if (freePagesList[baseAddr / 4096 + nPages] > 0) {
    freePagesList[baseAddr / 4096] += freePagesList[baseAddr / 4096 + nPages];
  }
}

void *allocPages(int32_t n) { return simpleAlloc(n); }
void freePages(void *ptr) { simpleDealloc(ptr); }