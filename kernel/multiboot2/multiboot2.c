#include "multiboot2.h"
#include <debug/print.h>
#include <stdint.h>

void *findMultiboot2Tag(uint32_t type) {
  multiboot2TagHeader *hdr = (void *)((uint32_t)MBI_ADDR + 8);
  while ((uint32_t)hdr < (uint32_t)MBI_ADDR + MBI_ADDR->totalSize) {
    if (hdr->type == type) {
      return hdr;
    }

    // align 8 bytes
    uint32_t sz = hdr->size;
    while (sz % 8 != 0) {
      sz++;
    }

    hdr = (void *)((uint32_t)hdr + sz);
  }

  return 0;
}

void ListTag() {
  multiboot2TagHeader *hdr = (void *)((uint32_t)MBI_ADDR + 8);
  while ((uint32_t)hdr < (uint32_t)MBI_ADDR + MBI_ADDR->totalSize) {
    printStr("tag = ");
    printUint(hdr->type);
    newline();

    // align 8 bytes
    uint32_t sz = hdr->size;
    while (sz % 8 != 0) {
      sz++;
    }

    hdr = (void *)((uint32_t)hdr + sz);
  }
}