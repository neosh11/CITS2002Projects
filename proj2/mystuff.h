#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/param.h>
#include  <dirent.h>
#include  <unistd.h>

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define eval(X) X
#define progArgs(...) {__VA_ARGS__, NULL} 

extern char **argumentsArray(int size,char **array);
extern char *locationCommand(char* loc, char * command);
