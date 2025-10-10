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
#define PATH_MAX 500
#define MAX_ARGS 256
#define MAX_INPUT 2048
#define SHELL_NAME "shell"
#define cwd_length 120
#define MAX_LEN 5

char *input_history[MAX_LEN];
int history_count = 0;

typedef struct history {
  int index;
  char *line;
  struct history *next;
} history;
void write_int(int a);

void history_add(history **head_ref, const char *line, int new_index) {

  history *new_node = (history *)malloc(sizeof(history));
  new_node->index = new_index;
  new_node->next = NULL;

  size_t len = strlen(line);
  new_node->line = (char *)malloc(MAX_INPUT + 1);
  strncpy(new_node->line, line, len);

  if (*head_ref == NULL) {
    *head_ref = new_node;
    return;
  }
  history *last = *head_ref;
  while (last->next != NULL) {
    last = last->next;
  }
  last->next = new_node;
}

int history_get(history *head, int current_index, int find_index) {

  history *current = head;
  if (current == NULL || find_index > current_index) { // nothing to write
    write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
    return 0;
  }
  int count;
  while (current->next != NULL) {
    if (current->index == find_index) {
      write_int(find_index);
      write_output("\t");
      write_output(current->line);
      write_output("\n");
      return 1;
    }
    current = current->next;
  }
  // write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));

  // write_output("--");
  // write_output(current->line);
  // write_output("--");
  return 0;
}
int history_get_ten(history *head, int current_index) {
  history *current = head;
  if (current == NULL) {
    write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
    return 0;
  }

  for (int i = 0; i < 11; i++) {
    history_get(current, current_index, current_index - i);
    if (current_index - i < 0)
      break;
  }
  return 1;
}
int history_get_ten_bhistad(history *head, int current_index) {
  // write_int(current_index);
  history *current = head;
  if (current == NULL) { // nothing to write
    write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
    return 0;
  }
  int find_index = current_index;
  int count = 0;
  while (count < 10) {
    current = head;
    while (current->next != NULL) { // prints one index
      if (current->index == (find_index - count)) {
        write_int(find_index - count);
        write_output("----<\t");
        write_output(current->line);
        write_output("\n");
        break;
      }

      current = current->next;
    }
    if (current_index == count)
      break;

    count++;
  }
  // write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));

  // write_output("--");
  // write_output(current->line);
  // write_output("--");
  return 0;
}

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
  char cwd[PATH_MAX];
  if (getcwd(cwd, cwd_length) != NULL) {
    // char msg[2048];
    // char *d = "$";
    // strcpy(msg, cwd);
    // strcat(msg, "$");
    // write_output(msg);
    write_output(cwd);
    write_output("$ ");
  } else {
    char *msg = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
  }
  // write(1, cwd, strlen(cwd));
}

void remove_oldest_record();
void print_history();
char *get_input();
void sigint_handler(int signo);
void write_int(int n) {
  char buf[12]; // enough for sign + 10 digits + '\0'
  int i = 0;
  if (n == 0) {
    write(STDOUT_FILENO, "0", 1);
    return;
  }
  if (n < 0) {
    write(STDOUT_FILENO, "-", 1);
    if (n == -2147483648) {
      write(STDOUT_FILENO, "2147483648", 10);
      return;
    }
    n = -n;
  }
  while (n > 0) {
    buf[i++] = (n % 10) + '0';
    n /= 10;
  }
  while (i > 0) {
    i--;
    write(STDOUT_FILENO, &buf[i], 1);
  }
}
static int is_internal(const char *cmd) {
  return (strcmp(cmd, "exit") == 0 || strcmp(cmd, "pwd") == 0 ||
          strcmp(cmd, "cd") == 0 || strcmp(cmd, "help") == 0 ||
          strcmp(cmd, "history") == 0 || strcmp(cmd, "!!") == 0 ||
          (cmd[0] == '!'));
}
static char *expand_tilde(const char *path, char *out, size_t cap) {
  if (!path || path[0] != '~') {
    // just copy
    strncpy(out, path ? path : "", cap);
    out[cap - 1] = '\0';
    return out;
  }
  const char *rest = path + 1; // after '~'
  const char *home = NULL;

  if (*rest == '/' || *rest == '\0') {
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_dir)
      home = pw->pw_dir;
  } else {
    // ~user not required by your spec; keep simple
    struct passwd *pw = getpwuid(getuid());
    if (pw && pw->pw_dir)
      home = pw->pw_dir;
  }

  if (!home)
    home = "";

  // build: home + rest
  if (*rest == '/')
    rest++; // skip leading slash, we add our own
  int n = snprintf(out, cap, "%s%s%s", home, (*rest) ? "/" : "", rest);
  if (n < 0 || (size_t)n >= cap) {
    // truncate silently
    out[cap - 1] = '\0';
  }
  return out;
}

void print_help_all(void) {
  write_output(FORMAT_MSG("cd", CD_HELP_MSG));
  write_output(FORMAT_MSG("exit", EXIT_HELP_MSG));
  write_output(FORMAT_MSG("pwd", PWD_HELP_MSG));
  write_output(FORMAT_MSG("help", HELP_HELP_MSG));
  write_output(FORMAT_MSG("history", HISTORY_HELP_MSG));
}
void print_help_one(const char *what) {
  if (strcmp(what, "exit") == 0) {
    write_output("exit: " EXIT_HELP_MSG "\n");
  } else if (strcmp(what, "pwd") == 0) {
    write_output("pwd: " PWD_HELP_MSG "\n");
  } else if (strcmp(what, "cd") == 0) {
    write_output(FORMAT_MSG("cd", CD_HELP_MSG));
  } else if (strcmp(what, "help") == 0) {
    write_output("help: " HELP_HELP_MSG "\n");
  } else {
    // external
    char buf[512];
    snprintf(buf, sizeof(buf), "%s: %s\n", what, EXTERN_HELP_MSG);
    write_output(buf);
  }
}
static int builtin_exit(char **argv, int argc) {
  if (argc > 1) {
    write_err(FORMAT_MSG("exit", TMA_MSG));
    return 0; // do not exit
  }
  _exit(0); // async-signal-safe exit
}

static int builtin_pwd(char **argv, int argc) {
  if (argc > 1) {
    write_err(FORMAT_MSG("pwd", TMA_MSG));
    return 0;
  }
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    write_err(FORMAT_MSG("pwd", GETCWD_ERROR_MSG));
    return -1;
  }
  write_output(cwd);
  write_output("\n");
  return 0;
}

static int builtin_cd(char **argv, int argc, char prevdir[PATH_MAX]) {
  if (argc > 2) {
    write_err(FORMAT_MSG("cd", TMA_MSG));
    return 0;
  }
  char cwd[PATH_MAX];
  if (!getcwd(cwd, sizeof(cwd))) {
    write_err(FORMAT_MSG("cd", GETCWD_ERROR_MSG));
    // still attempt chdir if possible
  }

  const char *target = NULL;
  char expanded[PATH_MAX];

  if (argc == 1) {
    struct passwd *pw = getpwuid(getuid());
    target = (pw && pw->pw_dir) ? pw->pw_dir : "/";
  } else if (strcmp(argv[1], "-") == 0) {
    if (prevdir[0] == '\0') {
      // no previous dir recorded; just ignore
      target = cwd;
    } else {
      target = prevdir;
    }
  } else if (argv[1][0] == '~') {
    target = expand_tilde(argv[1], expanded, sizeof(expanded));
  } else {
    target = argv[1];
  }

  if (chdir(target) != 0) {
    write_err(FORMAT_MSG("cd", CHDIR_ERROR_MSG));
    return -1;
  }
  // update prevdir
  if (cwd[0] != '\0') {
    strncpy(prevdir, cwd, PATH_MAX);
    prevdir[PATH_MAX - 1] = '\0';
  }
  return 0;
}

static int builtin_help(char **argv, int argc) {
  if (argc > 2) {
    write_err(FORMAT_MSG("help", TMA_MSG));
    return 0;
  }
  if (argc == 1) {
    print_help_all();
  } else {
    print_help_one(argv[1]);
  }
  return 0;
}
// history *head = NULL;
// int history_index = 0;
int main() { //---------------------------------------------------------------------------------------------------------------------------
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigint_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART; // auto-restart some syscalls when possible
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    // Fallback: ignore install error
  }
  // write(STDOUT_FILENO, ":\0", 2);
  char *argv[MAX_ARGS];
  char line[MAX_INPUT];
  char prevdir[PATH_MAX];

  char history_command[MAX_INPUT];
  char history_line[MAX_INPUT];
  int history_index = 0;
  history *head = NULL;
  // char *argv_history;
  //   88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
  //    fork process
  //  write_output("start:\n");
  short is_history = 0; // is history !! or !n triggered??
  for (;;) {
    ssize_t n = 0;
    //  fooooooooooooooooooooooooooooooooor
    reap_zombies();

    if (is_history == 1) {
      write_output(history_line);
      write_output("\n");
      strcpy(line, history_line);
      n = strlen(line);
    } else {

      print_prompt();
      n = read_input(line, sizeof(line));
    }
    is_history = 0;

    /*
        } else {
          write_output("\nhist line:");
          write_output(line);
          write_output("]]]]");
          break;
          strncpy(line, history_line, strlen(history_line));
          n = strlen(history_line);
        }
        is_history = 0;
    */
    // history_add(&head, line, history_index);
    // history_index++;
    // printf("{%s}", line);
    // write_output("\n[read input]: ");
    // write_int(n);

    // write_output("]]]]]\ncontains:(");
    // write_output(line);
    // write_output(")\n");
    // if (strlen(line) == 0) // strcpy(line,"")
    //   write_output("empty.");
    // if (n == 0) {
    //   write_output("NOx1\n");
    //  break;
    //  break;
    //}

    if (n == -1 || strlen(line) == 0) {
      // write_output("NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO\n");
      //  break;
      continue;
    } else {
      if (line[0] != '!') {
        history_add(&head, line, history_index);
        history_index++;
      }

      int is_bg = 0;
      int argc = parse_command(line, argv, &is_bg);
      // if ((argv[0][0] != '!')) {
      //   history_add(&head, line, history_index);
      //  history_index++;
      //}
      // write_int(argc);
      // write_output("<<<-----\n");
      if (argc == 0) {
        // write_output("[0]");
        continue;
      }

      if (is_bg == 1) {
        // write_output("[bg]");
      }
      if (argv[0][0] == '!') {
        if (n == 2) {
          write_err(FORMAT_MSG("shell", EXEC_ERROR_MSG));
          continue;
        }
        if (n > 2) {
          if (!(argv[0][1] == '0' || argv[0][1] == '1' || argv[0][1] == '2' ||
                argv[0][1] == '3' || argv[0][1] == '4' || argv[0][1] == '5' ||
                argv[0][1] == '6' || argv[0][1] == '7' || argv[0][1] == '8' ||
                argv[0][1] == '9' || argv[0][1] == '!')) {
            write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
            continue;
          }
        }
      }
      //]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
      if (is_internal(argv[0])) {
        if (strcmp(argv[0], "exit") == 0) {
          builtin_exit(argv, argc);
        } else if (strcmp(argv[0], "pwd") == 0) {
          builtin_pwd(argv, argc);
        } else if (strcmp(argv[0], "cd") == 0) {
          builtin_cd(argv, argc, prevdir);
        } else if (strcmp(argv[0], "help") == 0) {
          builtin_help(argv, argc);
        } else if (strcmp(argv[0], "history") == 0) {
          // history_add(&head, line, history_index);
          // history_index++;

          write_int(history_index - 1);
          write_output("\thistory\n");

          history_get_ten(head, history_index);
          continue;
        } else if (strcmp(argv[0], "!!") == 0) {
          if (head == NULL) {
            write_err(FORMAT_MSG("history", HISTORY_NO_LAST_MSG));
            continue;
          }
          // write_int(history_index - 1);
          history *find = head;
          // int find_index = 0;
          while (find->next != NULL) {
            //  if (find->index == (history_index - 1))
            // write_output("->");
            // write_output(find->line);if(find->index==)
            find = find->next;
            // if (find->next == NULL)
            //  break;
          }
          // write_output("->");

          ////////////////write_output(find->line);
          //      write_output("-------[][]\n");
          strcpy(history_line, find->line);
          is_history = 1;
          //    write_output("\nThis will run next:");
          //   write_output(history_command);
          /////////////////write_output("]\n");

          continue;
          // write_output("----\n");
          //  history_get(head, history_index, history_index - 1);
        } else if (argv[0][0] == '!') {
          // long long find_index=strtoll(*argv+1)
          //
          // write(STDOUT_FILENO, argv[0] + 1, strlen(argv[0]) -
          // 1);-------->write the rest of the line !!! argv[0] +1 !!!!
          char *end = NULL;
          // long long find_index = strtoll(argv[0] + 1, &end, 10);
          // if (*end != '\0') {
          //   write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
          // }
          int find_index = atoi(argv[0] + 1);
          // write_int((int)find_index);
          history *tofind = head;
          // int find_index = 0;
          if (find_index >= history_index) {
            write_err(FORMAT_MSG("history", HISTORY_INVALID_MSG));
            continue;
          }
          while (tofind->index != history_index) {
            //  if (find->index == (history_index - 1))
            // write_output("->");
            // write_output(find->line);if(find->index==)
            if (tofind->index == (int)find_index) {
              ///////////////write_output(tofind->line);
              ////////////////write_output("\n");
              strcpy(history_line, tofind->line);
              is_history = 1;

              break;
              continue;
            }
            tofind = tofind->next;
            // if (find->next == NULL)
            //  break;
          }
          // write_output("->");
          // if (tofind->index == (int)find_index) {
          //  write_output(tofind->line);
          //  break;
          //}
          // write_output(find->line);
          // write_output("\n");
        }
        continue;
      }

      //]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
      pid_t pid = fork();
      if (pid < 0) {
        write_err(FORK_ERROR_MSG);
        continue;
      }
      if (pid == 0) {
        // Child: restore default SIGINT so fg jobs get Ctrl-C
        struct sigaction sa_child;
        memset(&sa_child, 0, sizeof(sa_child));
        sa_child.sa_handler = SIG_DFL;
        sigemptyset(&sa_child.sa_mask);
        sigaction(SIGINT, &sa_child, NULL);
        // execl("sleep", "sleep", "0.1", NULL);
        execvp(argv[0], argv);

        // write_output("NOOOOOOOOOOOOOOOOOO YOU SUCN\n");
        char *msgerr = FORMAT_MSG("shell", EXEC_ERROR_MSG);

        write(STDERR_FILENO, msgerr, strlen(msgerr));
        // write_output("mosh");

        // write_int(errno);
        //  char *msg = "shell: unable to execute command\n";
        //  write(STDERR_FILENO, msg, strlen(msg));

        // write_output("exec fail\n");
        //  If we reach here, exec failed
        //  write_err(EXEC_ERROR_MSG);
        _exit(127);
      } else {

        // Parent
        if (!is_bg) {
          int status;
          for (;;) {
            pid_t w = waitpid(pid, &status, 0);
            if (w == pid)
              break;
            if (w < 0) {
              if (errno == EINTR)
                continue; // interrupted by SIGINT
              write_err(FORMAT_MSG(SHELL_NAME, WAIT_ERROR_MSG));
              break;
            }
          }
        } else {
          // Background: do not wait now; we did a zombie sweep above and will
          // again later
        }
      }
      ////]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
    }
  } // endfor
  // printf("\n\n\n\n%d", argc);
  //  88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
  return 0;
} //---------------------------------------------------------------------------------------------------------------------------------------

void sigint_handler(int signo) {
  (void)signo;
  write_output("\n");
  // const char *h1 = "exit: " EXIT_HELP_MSG "\n";
  // const char *h2 = "pwd: " PWD_HELP_MSG "\n";
  // const char *h3 = "cd: " CD_HELP_MSG "\n";
  // const char *h4 = "help: " HELP_HELP_MSG "\n";
  // const char *tip = "\n(Use '&' to run background)\n";
  print_help_all();

  //  write(STDOUT_FILENO, h1, strlen(h1));
  // write(STDOUT_FILENO, h2, strlen(h2));
  // write(STDOUT_FILENO, h3, strlen(h3));
  // write(STDOUT_FILENO, h4, strlen(h4));
  // write(STDOUT_FILENO, tip, strlen(tip));

  /* reprint prompt */
  print_prompt();
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
