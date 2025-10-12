#define _POSIX_C_SOURCE 200809L
#include "../include/io.h"
#include "../include/msgs.h"

ssize_t read_input(char *buffer, size_t size) {

  ssize_t bytes_read = read(STDIN_FILENO, buffer, size - 1);
  if (bytes_read == 0) {

    return -1;
  }
  if (bytes_read < 0) {

    write_err(FORMAT_MSG("shell", READ_ERROR_MSG));
    return -1;
  }
  if (bytes_read > 0) {
    if (strcmp(buffer, "\n") == 0) {
      // write_output("[AHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH]");
      return -1;
    }
    buffer[bytes_read] = '\0';
    char *newline = strchr(buffer, '\n');
    if (newline)
      *newline = '\0';
  }

  return bytes_read;
}

ssize_t write_err_2(const char *str) {

  size_t len = strlen(str);
  size_t bytes_written = 0;
  size_t tmp;
  while (bytes_written < len) {

    tmp = write(STDERR_FILENO, str + bytes_written, len - bytes_written);
    if (tmp < 0) {
      perror("write error;");
      exit(EXIT_FAILURE);
    }
    bytes_written += tmp;
  }
  return bytes_written;
}
ssize_t write_output(const char *str) {

  ssize_t n = write(STDOUT_FILENO, str, strlen(str));
  return n;
}
ssize_t write_err(const char *str) {
  ssize_t n = write(STDERR_FILENO, str, strlen(str));
  return n;
}

ssize_t write_output_2(const char *str) {

  size_t len = strlen(str);
  size_t bytes_written = 0;
  size_t tmp;
  while (bytes_written < len) {

    tmp = write(STDOUT_FILENO, str + bytes_written, len - bytes_written);
    if (tmp < 0) {
      perror("write error;");
    }
    bytes_written += tmp;
  }
  return bytes_written;
}
int parse_command(char *line, char **argv, int *is_bg) {

  *is_bg = 0; // not bg
  char *save_ptr = NULL;
  const char *delim = " \t\r\n";
  int argc = 0;
  char *token = NULL;

  while (argc < 255 && (token = strtok_r(line, delim, &save_ptr))) {
    // write_output("-->");
    // write_output(token);
    // write_output("\n");
    argv[argc] = token;
    argc++;
    line = NULL;
  }

  argv[argc] = NULL;
  // write_output(argv[argc]);
  //  if (argv[argc - 1] == NULL)
  //    write_output("\n");
  if (strcmp(argv[argc - 1], "&") == 0) {

    *is_bg = 1;
    argv[argc - 1] = NULL;
    argc--;
  }
  return argc;
}
// void write_int(int value){
//
//   char buffer[100];
//   // int len=snprintf
//
// }
