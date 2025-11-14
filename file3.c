#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  char *str = "Hello world!";
  char buf[12];

  int fd = open("tmp7", O_RDWR | O_CREAT, S_IRUSR | O_TRUNC);
  int res = write(fd, str, strlen(str));

  while (1) {
    sleep(30);
  }
  printf("\n");
}
