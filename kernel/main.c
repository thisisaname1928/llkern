#include "arch/interrupts/IDT.h"
#include "bootstrap.h"
#include "multiboot2/multiboot2.h"
#include <debug/print.h>
#include <mm/mm.h>
#include <stdint.h>
#include <utils/math.h>
#include <utils/mem/mem.h>

multiboot2MemoryMapTag *memmap;
uint32_t availableMemory = 0;
uint32_t kernelEndAddr;

int main() {
  initIDT32();

  kernelEndAddr = align((uint32_t)&KERNEL_END, 4096);

  printStr("Kernel was booted by ");
  multiboot2BootloaderNameTag *hdr =
      findMultiboot2Tag(MULTIBOOT2_BOOT_LOADER_NAME_TAG);

  printStr((char *)&hdr->name);
  newline();
  printHex(KERNEL_CS);
  newline();

  memmap = findMultiboot2Tag(MULTIBOOT2_MEMORY_MAP_TAG);
  multiboot2MemoryMapEntry *entry = memmap->entries;

  multiboot2ImageLoadPhysAddr *physAddrTag =
      findMultiboot2Tag(MULTIBOOT2_IMAGE_LOAD_PHYS_ADDR_TAG);

  printStr("Base Addr:  ");
  printHex(physAddrTag->loadBaseAddr);
  newline();

  printStr("kernel end: ");
  printHex((uint32_t)kernelEndAddr);
  newline();

  while ((uint32_t)entry < (uint32_t)memmap + memmap->size) {
    if (entry->type == MULTIBOOT2_MEMORY_AVAILABLE) {
      availableMemory += entry->length;
    }

    // reserved 0x0 mem block
    if (entry->baseAddr == 0x0) {
      entry->type = MULTIBOOT2_MEMORY_RESERVED;
    }

    // add kernel code memory to memmap
    if (entry->baseAddr == physAddrTag->loadBaseAddr) {
      entry->length =
          entry->length - ((uint32_t)kernelEndAddr - physAddrTag->loadBaseAddr);
      entry->baseAddr = (uint32_t)kernelEndAddr;
    }

    printStr("Base addr: 0x");
    printHex(entry->baseAddr);
    printStr(" length = ");
    printUint(entry->length);
    printStr(" type = ");
    printUint(entry->type);
    newline();

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }

  printStr("Available memory: ");
  printUint(availableMemory);
  newline();

  initPageAllocator();

  void *a1 = allocPages(1);
  void *a2 = allocPages(4);
  // freePages(a2);
  void *a3 = allocPages(1);
  printHex((uint32_t)a1);
  newline();
  printHex((uint32_t)a2);
  newline();
  printHex((uint32_t)a3);
  newline();
  return 0;
}