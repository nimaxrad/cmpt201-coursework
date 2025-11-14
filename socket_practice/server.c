#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

int main() {

  printf("Server:\n");

  // socket
  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    perror("SOcket failed!");
    exit(EXIT_FAILURE);
  }

  // bind
  struct sockaddr_un sockstruct {
    sockstruct.sun_family AF_UNIX;
    char sun_path[108];
  };

  if (bind(socket_fd, const struct sockaddr *addr, socklen_t addrlen) == -1) {

    perror("bind_failed");
    exit(EXIT_FAILURE);
  }

  return 0;
}
