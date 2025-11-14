#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUF_SIZE 64
#define MY_SOCK_PATH "tmp"
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sfd == -1)
    handle_error("socket");

  if (remove(MY_SOCK_PATH) == -1 &&
      errno != ENOENT) // No such file or directory
    handle_error("remove");

  struct sockaddr_un addr;
  memset(&addr, 0, sizeof(struct sockaddr_un));
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, MY_SOCK_PATH, sizeof(addr.sun_path) - 1);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1)
    handle_error("bind");

  if (listen(sfd, LISTEN_BACKLOG) == -1)
    handle_error("listen");

  for (;;) {
    int cfd = accept(sfd, NULL, NULL);
    if (cfd == -1)
      handle_error("accept");

    char buf[BUF_SIZE];
    ssize_t num_read;
    while ((num_read = read(cfd, buf, BUF_SIZE)) > 0) {
      if (write(STDOUT_FILENO, buf, num_read) != num_read)
        handle_error("write");

      if (num_read == -1)
        handle_error("read");
    }
    if (close(cfd) == -1)
      handle_error("close");
  }
}
