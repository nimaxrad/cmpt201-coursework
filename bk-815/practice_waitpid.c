#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int main() {
  //  printf("start pid=%d, parent=%d\n", getpid(), getppid());
  printf("start\n");
  pid_t pid = fork();
  if (pid == 0) {
    printf("Child\n");
    printf("current ppid:%d, parent pid:%d\n", getpid(), getppid());
    sleep(20);
    // if (execl("/oousr/bin/ls", "usr/bin/ls", "-a", "-l", NULL) == -1) {
    //   printf("execl failed\n");
    //   perror("execl");
    //   exit(EXIT_FAILURE);
    // }

  } else {
    int wstatus = 0;
    if (waitpid(pid, &wstatus, 0) == -1) {
      perror("failure");
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(wstatus)) {
      printf("gooood\n");
    } else {

      printf("baddd\n");
    }
  }
  printf("end\n");
}
