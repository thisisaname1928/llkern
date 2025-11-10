#include "mem.h"
#include <stddef.h>
#include <stdint.h>

void memcpy(void *dest, const void *src, uint32_t n) {
  char *ddest = (char *)dest, *ssrc = (char *)src;

  if (src > dest)
    for (uint32_t i = 0; i < n; i++)
      ddest[i] = ssrc[i];
  else if (dest > src)
    for (uint32_t i = n; i > 0; i--)
      ddest[i - 1] = ssrc[i - 1];
}

void memmove(void *dest, const void *src, size_t n) {
  char *ddest = (char *)dest, *ssrc = (char *)src;

  for (uint32_t i = 0; i < n; i++) {
    ddest[i] = ssrc[i];
    ssrc[i] = 0;
  }
}

int memcmp(const void *ptr1, const void *ptr2, size_t num) {
  char *p1 = (char *)ptr1, *p2 = (char *)ptr2;

  for (uint32_t i = 0; i < num; i++) {
    if (p1[i] != p2[i]) {
      if (p1[i] > p2[i])
        return 1;
      else
        return -1;
    }
  }

  return 0;
}

uint32_t align(uint32_t addr, uint32_t boundary) {
  return (addr + (boundary - 1)) & (~(boundary - 1));
}