#include <stdint.h>
#include <utils/mem/mem.h>

volatile uint16_t *buffer = (uint16_t *)0xb8000;

int x = 0, y = 0;
uint32_t color = 0xf;
const int MAX_WIDTH = 80, MAX_HEIGHT = 25, SINGLE_CHAR_SIZE = 2;

void scroll() {
  memmove((void *)buffer, (void *)&buffer[80],
          (MAX_HEIGHT - 1) * MAX_WIDTH * 2);
  y = MAX_HEIGHT - 1;
  x = 0;
}

void newline() {
  y++;
  x = 0;

  if (y >= MAX_HEIGHT) {
    scroll();
  }
}

void putchar(int c) {
  buffer[y * MAX_WIDTH + x] = ((color & 0xff) << 8) | (c & 0xff);

  // inc x y
  x++;
  if (x >= MAX_WIDTH) {
    newline();
  }
}

void printStr(const char *str) {
  while (*str != 0) {

    if (*str == '\n')
      newline();
    else if (*str == '\r')
      x = 0;
    else
      putchar(*str);

    str++;
  }
}

void printUint(uint32_t n) {
  if (n == 0) {
    printStr("0");
    return;
  }

  char buff[21];
  buff[20] = 0;
  int c = 19;

  while (n != 0) {
    buff[c] = n % 10 + 48;
    n /= 10;
    c--;
  }

  printStr(&buff[c + 1]);
}

const char hexTrans[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

void printHex(uint32_t n) {
  if (n == 0) {
    printStr("0");
    return;
  }

  char buff[21];
  buff[20] = 0;
  int c = 19;

  while (n != 0) {
    buff[c] = hexTrans[n % 16];
    n /= 16;
    c--;
  }

  printStr(&buff[c + 1]);
}