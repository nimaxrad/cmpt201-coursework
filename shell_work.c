#define _POSIX_C_SOURCE 200809L
// Q | how to compile the tester and use it correctly?? am i using the right
// CMAKE file?
// should i use write instead of printf?
// how many records should the history array keep?
#include "../include/io.h"
#include "../include/msgs.h"
#include <errno.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define MAX_LINE 4096
#define MAX_ARGS 256
#define MAX_INPUT 2048

#define cwd_length 120
#define MAX_LEN 5
char cwd[cwd_length];
char *input_history[MAX_LEN];
int history_count = 0;

void reap_zombies(void) {
  int status;
  for (;;) {

    pid_t r = waitpid(-1, &status, WNOHANG);
    if (r > 0)
      continue;
    if (r == 0)
      break;
    if (r < 0) {
      if (errno == EINTR)
        continue;
      if (errno == ECHILD)
        break;
      write_err(FORMAT_MSG("shell", WAIT_ERROR_MSG));
      break;
    }
  }
}
void print_prompt(void) {
  if (getcwd(cwd, cwd_length) != NULL)
    strncat(cwd, "$", 1);
  else {
    char *msg = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
    write(1, msg, strlen(msg));
  }
  write(1, cwd, strlen(cwd));
}

void add_to_history(char *input);

void remove_oldest_record();
void print_history();
char *get_input();

int main() { //---------------------------------------------------------------------------------------------------------------------------
  /*
   * 1) get input
   * 2) process the input with strtok_r
   * 3) fork and run the child process,
   * 4) wait on the process to finish
   * 5) Setup a signal handler to stop ctrl c
   * 6)
   *
   * */
  // while (1) {
  //   print_prompt();
  //   char *input = get_input();
  //
  //  add_to_history(input);
  //    if (strcmp(input, "print") == 0) {
  //      print_history();
  //    }
  //  }

  write(STDOUT_FILENO, ":\0", 2);

  char input[MAX_INPUT];
  ssize_t bytes_read;
  // bytes_read = read(STDIN_FILENO, input, 100);

  // if (bytes_read)
  //   printf("success: [%s] bytes read [%lu]\n", input, bytes_read);
  bytes_read = read_input(input, sizeof(input));
  if (bytes_read > 0)
    // printf("success: [%s] bytes read [%lu]\n", input, bytes_read);
    write_output(input);
  write_output("\n");
  char *arglist[MAX_ARGS];
  // parse_command(char *line, char **argv, bool *isbg);
  int isbg = 0;
  write_output("\n-------------------\nparse:");
  int argc = parse_command(input, arglist, &isbg);
  // 88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
  //  fork process
  //

  printf("\n\n\n\n%d", argc);
  // 88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
  return 0;
} //---------------------------------------------------------------------------------------------------------------------------------------

char *get_input() {
  char *buffer = NULL;
  size_t bufsize = 0;
  size_t len = getline(&buffer, &bufsize, stdin);
  if (len == -1) {
    exit(1);
  }
  // Remove the newline character at the end of the input
  buffer[len - 1] = '\0';
  return buffer;
}

void add_to_history(char *input) {
  if (history_count >= MAX_LEN) {
    remove_oldest_record();
  }
  input_history[history_count] = input;
  history_count++;
}

void remove_oldest_record() {
  if (history_count > 0) {
    // Free the memory of the oldest record
    free(input_history[0]);
    for (int i = 1; i < history_count; i++) {
      input_history[i - 1] = input_history[i];
    }
    history_count--;
  }
}

void print_history() {
  for (int i = 0; i < history_count; i++) {
    // printf("%s\n", input_history[i]);
    // const char *history_line=
    // strnmpt(input_history[i],"\n");
    write(STDOUT_FILENO, input_history[i], strlen(input_history[i]));
  }
}
