#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MY_SOCK_PATH "tmp"
#define BUF_SIZE 64

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, MY_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    handle_error("connect");

  char buf[BUF_SIZE];
  ssize_t num_read;
  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1)
    if (write(sfd, buf, num_read) != num_read)
      handle_error("write");

  if (num_read == -1)
    handle_error("read");

  exit(EXIT_SUCCESS);
}
