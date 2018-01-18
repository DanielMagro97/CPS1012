#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_INTERVAL 25

int main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "Incorrect number of arguments; usage: presblock [pid]\n");
    exit(EXIT_FAILURE);
  }

  // Convert text pid to int
  int proc_pid = atoi(argv[1]);

  // Check if pid is a sane value
  if (proc_pid > 0) {
    for(;;) {
      // AGENT Smith:
      // rand() is the worst PRNG ever; FFS use something else!
      int delay = rand() % MAX_INTERVAL;

      // Delay and send signal
      printf("Delaying for %d seconds...\n", delay);
      sleep(delay);
      kill(proc_pid, SIGALRM);
    }
  } else {
    fprintf(stderr, "Invalid pid! Exiting...\n");
    exit(EXIT_FAILURE);
  }

  // Unreachable code
  exit(EXIT_SUCCESS);
}
