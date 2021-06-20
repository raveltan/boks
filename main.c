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

/******
 *  Functions declaration  *
 ******/

void run();
// Function to be used with clone
int startAction(void *args);

/******
 *  Entry Point  *
 ******/

int main() {
  // TODO: add argument checking and pass to run
  run();
  return 0;
}

void run() {
  printf("Running %s\n", "/bin/bash");
  int namespaces = CLONE_NEWUTS;
  // CLONE require CAP_SYS_ADMIN thus needs to be run as root.
  pid_t child_pid =
      clone(startAction, malloc(4096) + 4096, SIGCHLD | namespaces, NULL);
  if (child_pid == -1) {
    perror("Clone error");
    exit(1);
  }
  waitpid(child_pid, NULL, 0);
}

int startAction(void *args) {
  char *argv[] = {"/bin/bash", NULL};
  int result = execv("/bin/bash", argv);
  if (result != 0) {
    perror("error");
  }
  exit(result);
}
