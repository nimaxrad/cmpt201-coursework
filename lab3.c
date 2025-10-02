// LAB2
// NIMA SALEHIRAD
#include <stdbool.h> // Nima Salehirad LAB 1
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> // added for tT
#include <sys/wait.h>
#include <unistd.h>

int main() {
  for (;;) {

    printf("Enter input: ");
    char *buff = NULL;
    char *fullpath = malloc(sizeof(char) * 100);
    size_t size = 0;
    char *path = NULL;
    if (getline(&buff, &size, stdin) != -1L) {
      char *input_str = buff;
      char *delim = " /\n\t\r";
      char *token = NULL;
      char *saveptr = NULL;
      strcpy(fullpath, buff);
      while ((token = strtok_r(input_str, delim, &saveptr))) {

        //        printf("token:'%s'\n", token);
        input_str = NULL;
      }
      // strcpy(fullpath, buff);

      int v = strcspn(fullpath, "\n");
      fullpath[v] = '\0';
      //      fullpath[strcspn(fullpath, "\n")] = '\0';
      // printf("::%d", v);
      //      printf("strong input: [%s]\n", fullpath); // got the path without
      //      \n
      // -- re
      pid_t pid = fork();
      if (pid < 0) {
        perror("fork failed\n");
      } else if (pid == 0) {
        execl(fullpath, fullpath, (char *)NULL);
        perror("execl failed\n");
        exit(0);
      } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
          perror("waitpid failed\n");
        }
      }
    } else {
      printf("getline failure.\n");
      free(buff);
    }
    free(buff);
    free(fullpath);
  }
  return 0;
}
