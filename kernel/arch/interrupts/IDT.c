#include "IDT.h"
#include "bootstrap.h"
#include <debug/print.h>
#include <stdint.h>

volatile IDT32 __attribute__((
    aligned(4096))) globalIDT32[256]; // align for faster accessing

volatile IDT32R globalIDT32R;

void setIDT32(IDT32 *IDT, void *base, uint8_t attribute) {
  uint32_t baseAddr = (uint32_t)base;

  IDT->offset1 = baseAddr & 0xffff;
  IDT->offset2 = (baseAddr >> 16) & 0xffff;
  IDT->attribute = attribute;
  IDT->cs = KERNEL_CS;
}

void testIntterupt(uint32_t n) {
  printStr("UHH ERR: ");
  printUint(n);
}

void unHandledException() {
  printStr("UHH ERR UNHAND\n");
  __asm__ __volatile__("jmp %0" : : "r"(callIretd));
}

void invalidOpcodeException() {
  printStr("Invalid Opcode!\n");
  while (1) {
    __asm__("hlt");
  }
}

void divisionException() {
  printStr("Division Error!\n");
  while (1) {
  }
}

void initIDT32() {

  for (int i = 0; i < 256; i++) {
    if (i < 32) {
      setIDT32((IDT32 *)&globalIDT32[i], unHandledException,
               IDT32_PRESENT | IDT32_TRAP_GATE | IDT32_DPL(0));
    } else {
      setIDT32((IDT32 *)&globalIDT32[i], unHandledException,
               IDT32_PRESENT | IDT32_INTERRUPT_GATE | IDT32_DPL(0));
    }
  }

  globalIDT32R.offset = (uint32_t)&globalIDT32;
  globalIDT32R.size = sizeof(IDT32) * 256;

  setIDT32((IDT32 *)&globalIDT32[6], invalidOpcodeException,
           IDT32_PRESENT | IDT32_TRAP_GATE | IDT32_DPL(0));
  setIDT32((IDT32 *)&globalIDT32[0], divisionException,
           IDT32_PRESENT | IDT32_TRAP_GATE | IDT32_DPL(0));

  loadIDT();
}