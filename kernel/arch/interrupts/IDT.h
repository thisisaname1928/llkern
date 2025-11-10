#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT32_INTERRUPT_GATE 0xE
#define IDT32_TRAP_GATE 0xF
#define IDT32_PRESENT (1 << 7)
#define IDT32_DPL(dpl) (dpl << 5)

typedef struct __attribute__((packed)) {
  uint16_t size;
  uint32_t offset;
} IDT32R;

typedef struct __attribute__((packed)) {
  uint16_t offset1;
  uint16_t cs;
  uint8_t reserved;
  uint8_t attribute;
  uint16_t offset2;
} IDT32;

extern volatile IDT32 globalIDT32[256];
extern volatile IDT32R globalIDT32R;

void setIDT32(IDT32 *IDT, void *base, uint8_t attribute);
void initIDT32();
void loadIDT();
void callIretd();

#endif