#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
int main() {
  printf("CLIENT: \n");

  int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    perror("socket failed!");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_un sockstruct;
  sockstruct.sun_family = AF_UNIX;
  snprintf(sockstruct.sun_path, 108, "socket_fun");

  char is_connected = 0;

  while (!is_connected) {
    sleep(2);
    if (connect(socket_fd, (struct sockaddr *)&sockstruct,
                sizeof(struct sockaddr_un)) == -1) {
      perror("connect failed");
      // exit(EXIT_FAILURE);
    } else {
      is_connected = 1;
    }
  }
  //
  //
  //
  // connet
  // write
  char *msg = "[+]a message from client!";
  int bytes_written = write(socket_fd, msg, strlen(msg));
  if (bytes_written == -1) {
    perror("write failed");
    exit(EXIT_FAILURE);
  }
  for (int i; i < 10; i++) {
    const int size = 1024;
    char buff[size];
    read(socket_fd, buff, size);
    sleep(1);
  }
  close(socket_fd);
  // close
  //
  return 0;

  printf("CLIENT: \n");
}
