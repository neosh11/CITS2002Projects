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

    printf("%s", line);
    if (pid == 0) {
      fclose(input);
      exit(0);
    } else {
      waitpid(pid, &x, 0);
    }
  }

  fclose(input);
}