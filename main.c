// clone
#define _GNU_SOURCE
#include <sched.h>

// IO
#include <stdio.h>
// malloc
#include <stdlib.h>

// waitpid
#include <sys/wait.h>
// execv
#include <unistd.h>

#include <string.h>

/******
 *  Functions declaration  *
 ******/

void run(int argc, char **argv);
// Function to be used with clone
int startAction(void *args);
// Program manual
void manual();

/******
 *  Structs  *
 ******/

struct run_data {
  int size;
  char **args;
};

/******
 *  Entry Point  *
 ******/

int main(int argc, char **argv) {
  if (argc < 2) {
    manual();
    exit(0);
  }
  if (strcmp(argv[1], "run") || strcmp(argv[1], "r")) {
    if (argc < 3) {
      perror("Please speficy a command to run");
      exit(1);
    }
    run(argc, argv);
  }
  return 0;
}

// Handle running command on container
void run(int argc, char **argv) {
  // Flags to be used when cloning the processs
  int namespaces = CLONE_NEWUTS;
  // clone the process with the namepaces flags and run startAction inside of
  // it. CLONE require CAP_SYS_ADMIN thus needs to be run as root.
  struct run_data data;
  data.args = argv;
  data.size = argc;
  void *args = (void *)&data;

  pid_t child_pid =
      clone(startAction, malloc(4096) + 4096, SIGCHLD | namespaces, args);
  if (child_pid == -1) {
    perror("Clone error");
    exit(1);
  }
  waitpid(child_pid, NULL, 0);
}

int startAction(void *args) {
  struct run_data *data = (struct run_data *)args;
  char **execution_data = malloc((data->size - 2) * sizeof(char **));
  memcpy(execution_data, data->args + 2, (data->size - 2) * sizeof(char **));
  int result = execv(data->args[2], execution_data);
  if (result != 0) {
    perror("error");
  }
  exit(result);
}

void manual() {
  // Add program manual here
}
