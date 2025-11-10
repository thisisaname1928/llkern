#include <stdint.h>

int32_t abs(int32_t x) {
  if (x < 0) {
    return x * -1;
  }

  return x;
}