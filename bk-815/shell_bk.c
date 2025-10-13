#define _POSIX_C_SOURCE 200809L
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define cwd_length 100
char cwd[cwd_length];
void print_prompt(void) {
  getcwd(cwd, cwd_length);
  printf("%s$", cwd);
}
int main(void) { print_prompt(); }
