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
#include <signal.h>


//#define PATH_MAX 1016 //Defined in sys/limits

#define foreach(i,x,y) for(int i = x; i < y; i++)
#define check(X) printf("CHECK %d\n", X);
#define printS(X) printf("%s\n", X)




extern char **argumentsArray(int size,char **array);
extern char *locationCommand(char* loc, char * command);
extern char *locationOfProg(char * relative);

extern int childrenArray[100];
extern int childCount;

extern char *PROGLOCATION;

typedef struct timeval TIMEVAL;

//****************************************
//************ ARRAY LIST ****************
typedef struct node
{
    char * path;
    struct node *ptr;
} NODE;

typedef struct List
{
    NODE *front;
    NODE *temp;
    NODE **end;
} LIST;

extern LIST * pathList;
extern LIST * cdList;

//************************************


extern void initializeList(LIST **list);
extern void enqueue(LIST **list, char * paths);
extern void delete(LIST **list);
extern void resetHead(LIST **list);
extern void next(LIST **list);
extern void display(LIST **list);//for debugging



