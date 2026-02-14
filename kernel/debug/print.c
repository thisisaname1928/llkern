
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
  if (c == '\n') {
    newline();
    return;
  }

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

void kprintf(const char *str, ...) {
  __builtin_va_list argsList;

  __builtin_va_start(argsList, str);
  uint32_t idx = 0;
  while (str[idx] != 0) {
    switch (str[idx]) {
    case '%':
      switch (str[idx + 1]) {
      case 'u':;
        uint32_t res = __builtin_va_arg(argsList, uint32_t);
        printUint(res);
        break;
      case 's':;
        char *sres = __builtin_va_arg(argsList, char *);
        if (sres != NULL) {
          printStr(sres);
        }
        break;
      case 'x':;
        res = __builtin_va_arg(argsList, uint32_t);
        printHex(res);
        break;
      default:
        putchar('%');
        putchar(str[idx + 1]);
        break;
      }
      idx += 2;
      break;
    default:
      putchar(str[idx]);
      idx++;
      break;
    }
  }

  __builtin_va_end(argsList);
}