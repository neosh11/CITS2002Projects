#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/param.h>
#include  <dirent.h>
#include  <unistd.h>
#include <sys/wait.h>

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define check(X) printf("CHECK %d\n", X);
#define printS(X) printf("%s\n", X)

extern char **argumentsArray(int size,char **array);
extern char *locationCommand(char* loc, char * command);

typedef struct node
{
    char * path;
    struct node *ptr;
} PathList;

//Identifier nodes for reference
extern PathList *front;
extern PathList *temp;

void initializeList(void);
void enqueue(char * paths);
void display(void);
void delete(void);
