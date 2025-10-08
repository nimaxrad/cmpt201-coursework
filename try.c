#include <stdio.h>  // for snprintf()
#include <string.h> // for strlen()
#include <unistd.h> // for write()

void write_int(int value) {
  char buffer[32]; // enough for a 64-bit int in decimal
  int len = snprintf(buffer, sizeof(buffer), "%d", value);
  write(STDOUT_FILENO, buffer, len);
}

int main(void) {

  write_int(32);
  return 0;
}
