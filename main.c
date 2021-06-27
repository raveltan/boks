// clone
#define _GNU_SOURCE
#include <sched.h>

// IO
#include <stdio.h>
// malloc, getenv
#include <stdlib.h>

// waitpid
#include <sys/wait.h>
// execv,sethostname
#include <unistd.h>

// mount
#include <sys/mount.h>
#include <sys/stat.h>

#include <string.h>

/******
 *  Functions declaration  *
 ******/

void run(int argc, char **argv);
// Function to be used with clone
int processRun(void *args);
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
      printf("Please speficy a command to run.\n");
      exit(1);
    }
    char *data = getenv("BOKS_IMAGE");
    if (!data) {
      printf(
          "Please make sure that filesystem path is defined by BOKS_IMAGE env "
          "variable.\n");
      exit(1);
    }
    run(argc, argv);
  }
  if (strcmp(argv[1], "help") || strcmp(argv[1], "h")) {
    manual();
  }
  return 0;
}

// Handle running command on container
void run(int argc, char **argv) {
  // Flags to be used when cloning the processs
  //  CLONE_NEWUTS provides hostname and NIS Domain name isolation
  //  CLONE_NEWPID alllows created container to have it's own PID namespace
  //  -> this will not alter ps result as it gathers info from /proc
  //  CLONE_NEWNS mount a new namespace rather than living in the same one
  //  -> with it's parent
  int namespaces = CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS;
  // clone the process with the namepaces flags and run startAction inside of
  // it. CLONE require CAP_SYS_ADMIN thus needs to be run as root.
  struct run_data data;
  data.args = argv;
  data.size = argc;
  void *args = (void *)&data;

  pid_t child_pid =
      clone(processRun, malloc(4096) + 4096, SIGCHLD | namespaces, args);
  if (child_pid == -1) {
    perror("Process clone error");
    exit(1);
  }
  // Wait for child_pid to die (await)
  waitpid(child_pid, NULL, 0);
}

int processRun(void *args) {
  // TODO: generate container hostname

  // Set container hostname
  char *hostname = "boks";
  int result = sethostname(hostname, sizeof(hostname));

  // setup cgroup
  char *cgroup_path = "sys/fs/cgroup/pids/boks";
  mkdir(cgroup_path, 0755);
  // Max pids
  FILE *max_pid_file = fopen(strcat(cgroup_path, "/pids.max"), "w+");
  if (fputs("10", max_pid_file) < 0) {
    printf("Unable to set max pids cgroup");
    exit(1);
  }
  fclose(max_pid_file);

  FILE *notify_on_release_file =
      fopen(strcat(cgroup_path, "/notify_on_release"), "w+");
  if (fputs("1", notify_on_release_file) < 0) {
    printf("Unable to set notify on release cgroup");
    exit(1);
  }
  fclose(notify_on_release_file);

  FILE *cgroup_procs_file = fopen(strcat(cgroup_path, "/cgroup.procs"), "w+");
  // Get current PID
  int process_id = getpid();
  char pid_string[30];
  sprintf(pid_string, "%d", process_id);
  if (fputs(pid_string, cgroup_procs_file) < 0) {
    printf("Unable to set cgroup procs");
    exit(1);
  }
  fclose(cgroup_procs_file);

  // Set base directory to those of BOKS_IMAGE
  char *new_root_path = getenv("BOKS_IMAGE");
  if (!new_root_path) {
    printf("Unable to get BOKS_IMAGE env variable.\n");
    exit(1);
  }
  if (chroot(new_root_path) < 0) {
    printf("Unable to set filesystem root, make sure that %s is "
           "correct.\n",
           new_root_path);
    exit(1);
  }
  // change to /
  if (chdir("/") < 0) {
    printf("Unable to switch to root dir of image.\n");
    exit(1);
  }

  // Mount proc to enable process data sharing between kernel and userspace
  if (mount("/proc", "proc", "proc", 0, "") < 0) {
    printf("Unable to mount proc.\n");
    exit(1);
  }

  // run command
  struct run_data *data = (struct run_data *)args;
  char **execution_data = malloc((data->size - 2) * sizeof(char **));
  memcpy(execution_data, data->args + 2, (data->size - 2) * sizeof(char **));
  result = execv(data->args[2], execution_data);
  if (result != 0) {
    printf("Unable to execute command\n");
  }
  umount("/proc");
  exit(result);
}

void manual() {
  // TODO: Add program manual here
}
