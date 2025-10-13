#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int main() {
  // printf("start pid=%d, parent=%d\n", getpid(), getppid());
  printf("start\n");
  pid_t pid = fork();
  if (pid == 0) {
    printf("CHILD\n");
    // sleep(10);
    printf("waiting...\n");
    sleep(5);
    char *args[] = {"/bin/ls", "-a", "-l", "-h", NULL};
    if (execv("/biun/ls", args) == -1) {
      perror("----exexcv error---");
      exit(EXIT_FAILURE);
    }
  } else {
    printf("Parent!\n");
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
      perror("waitpid");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(wstatus)) {
      printf("good:%d\n", WIFEXITED(wstatus));
    } else {
      printf("bad\n");
    }
  }
  printf("end\n");
}
