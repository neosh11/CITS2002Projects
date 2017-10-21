#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/param.h>
#include  <dirent.h>
#include  <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    int pipe1[2];

    if((pipe(pipe1)) != 0)
    {
        printf("FAIL");
    }

    switch(fork())
    {
    case -1:

        printf("FAIL");
    case 0:
        close(pipe1[0]);
        dup2(pipe1[1], 1);

        execv("/bin/ls", (char* []) {"ls", NULL});
    default:
        wait(NULL);
        close(pipe1[1]);
        dup2(pipe1[0], 0);
        execv("/usr/bin/sort", (char* []) {"sort", NULL});
    }
    return 0;
}