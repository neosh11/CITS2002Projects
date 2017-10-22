#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/stat.h>
#include  <sys/param.h>
#include  <dirent.h>
#include  <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <limits.h>
#include <fcntl.h>

//#define PATH_MAX 1016 //Defined in sys/limits

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define check(X) printf("CHECK %d\n", X);
#define printS(X) printf("%s\n", X)


extern char **argumentsArray(int size,char **array);
extern char *locationCommand(char* loc, char * command);
char *locationOfProg(char * relative);

extern char *PROGLOCATION;

typedef struct node
{
    char * path;
    struct node *ptr;
} PathList;

typedef struct timeval TIMEVAL;

//************************************
//Identifier nodes for reference
extern PathList *front;
extern PathList *temp;

extern void initializeList(void);
extern void enqueue(char * paths);
extern void delete(void);

extern void resetHead(void);
extern void next(void);

extern void display(void);//for debugging


