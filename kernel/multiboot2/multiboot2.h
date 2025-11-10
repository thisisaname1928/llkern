#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include <stdint.h>

#define MULTIBOOT2_BOOT_LOADER_NAME_TAG 2
#define MULTIBOOT2_MEMORY_MAP_TAG 6
#define MULTIBOOT2_IMAGE_LOAD_PHYS_ADDR_TAG 21

#define MULTIBOOT2_MEMORY_AVAILABLE 1
#define MULTIBOOT2_MEMORY_RESERVED 2
#define MULTIBOOT2_MEMORY_ACPI_RECLAIMABLE 3
#define MULTIBOOT2_MEMORY_NVS 4
#define MULTIBOOT2_MEMORY_BADRAM 5

typedef struct {
  uint32_t totalSize;
  uint32_t reserved;
} multiboot2InformationHeader;

typedef struct {
  uint32_t type;
  uint32_t size;
} multiboot2TagHeader;

typedef struct {
  uint32_t type;
  uint32_t size;
  char *name;
} multiboot2BootloaderNameTag;

typedef struct {
  uint32_t baseAddr;
  uint32_t reserved;
  uint32_t length;
  uint32_t reserved2;
  uint32_t type;
  uint32_t reserved3;
} multiboot2MemoryMapEntry;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t entrySize;
  uint32_t entryVersion;
  multiboot2MemoryMapEntry entries[];
} multiboot2MemoryMapTag;

typedef struct {
  uint32_t type;
  uint32_t size;
  uint32_t loadBaseAddr;
} multiboot2ImageLoadPhysAddr;

extern multiboot2MemoryMapTag *memmap;

extern multiboot2InformationHeader *MBI_ADDR;
void *findMultiboot2Tag(uint32_t type);
void ListTag();

#endif