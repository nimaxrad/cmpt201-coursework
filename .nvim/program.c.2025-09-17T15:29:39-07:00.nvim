#include <stdio.h>
#include <unistd.h>

int main() {

  execl("/usr/bin/ls", "ls -a", NULL);
  fork();

  execl("/usr/bin/ls", "ls -a -l -h", NULL);
  printf("%d\n", getpid());
  return 0;
}
