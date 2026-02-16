#include "arch/interrupts/IDT.h"
#include "bootstrap.h"
#include "multiboot2/multiboot2.h"
#include <arch/io.h>
#include <debug/print.h>
#include <mm/mm.h>
#include <mm/slab.h>
#include <stdint.h>
#include <utils/math.h>
#include <utils/mem/mem.h>

multiboot2MemoryMapTag *memmap;
uint32_t availableMemory = 0;
uint32_t kernelEndAddr;
uint32_t metadataSz; // in 4KB page
void *metadataPtr = NULL;

void initKernel() {
#define TMP_BUFFER_SZ 4096
  TmpMemAllocator allocator;
  char stackBuffer[TMP_BUFFER_SZ];
  initTmpMemAllocator(&allocator, stackBuffer, TMP_BUFFER_SZ);
  curTmpMemAllocator = &allocator;

  initBuddyAllocator();

  // end
  curTmpMemAllocator = NULL;
}

int main() {
  initIDT32();

  kernelEndAddr = align((uint32_t)&KERNEL_END, 4096);

  printStr("Kernel was booted by ");
  multiboot2BootloaderNameTag *hdr =
      findMultiboot2Tag(MULTIBOOT2_BOOT_LOADER_NAME_TAG);

  printStr((char *)&hdr->name);
  newline();

  memmap = findMultiboot2Tag(MULTIBOOT2_MEMORY_MAP_TAG);
  multiboot2MemoryMapEntry *entry = memmap->entries;

  multiboot2ImageLoadPhysAddr *physAddrTag =
      findMultiboot2Tag(MULTIBOOT2_IMAGE_LOAD_PHYS_ADDR_TAG);

  // loop to calculate neccessary value
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

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }

  // kernel will get 0.5% of available pages for page allocator data
  metadataSz = (uint32_t)(availableMemory / 4096) * (0.5 / 100);
  kprintf("use %u pages for metadata storing\n", metadataSz);

  // modify the memmap
  entry = memmap->entries;
  while ((uint32_t)entry < (uint32_t)memmap + memmap->size) {
    if (entry->type == MULTIBOOT2_MEMORY_AVAILABLE &&
        entry->length > metadataSz * 4096) {
      // ok
      metadataPtr = (void *)entry->baseAddr;

      entry->baseAddr += entry->baseAddr;
      entry->length -= metadataSz * 4096;
    }

    entry = (void *)((uint32_t)entry + memmap->entrySize);
  }

  if (metadataPtr == NULL) {
    return -1;
  }

  printStr("Base Addr:  ");
  printHex(physAddrTag->loadBaseAddr);
  newline();

  printStr("kernel end: ");
  printHex((uint32_t)kernelEndAddr);
  newline();

  printStr("Available memory: ");
  printUint(availableMemory);
  newline();

  SlabBlockHeader *metadataMem = metadataPtr;
  initNewSlabBLock(metadataMem, metadataSz * 4096);

  void *a = slabAlloc(metadataMem, 4);
  kprintf("---------------------------------\n");
  void *b = slabAlloc(metadataMem, 4);

  kprintf("blockSz = %u, a = %u, b = %u\n", metadataMem->blockSz, a, b);
  // initKernel();

  return 0;
}