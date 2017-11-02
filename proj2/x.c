#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/wait.h>


int main() {
  FILE *input = fopen("test.c", "r");


  char line[1000];
  int x;

  while (!feof(input)) {
    fgets(line, 1000, input);

    int pid = fork();

    if (pid == 0) {
      printf("%s", line);
      exit(0);
    } else {
      waitpid(pid, &x, 0);
    }
  }

  fclose(input);
}