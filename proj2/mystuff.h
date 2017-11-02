#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>


//MACROS FOR DEBUGGING
#define foreach(i,x,y) for(int i = x; i < y; i++)
#define check(X) printf("CHECK %d\n", X);
#define printS(X) printf("%s\n", X)

//*************************************************************
//********PROGRAM RELATED FEATURES*****************************
//*************************************************************

extern char *PROGLOCATION; //Stores the location of myshell
typedef struct timeval TIMEVAL; //Shorthand for struct timeval

extern char **argumentsArray(int size,char **array); //Converts {progname, a, b, ..., x} to {a, b, ..., x}
extern char *locationCommand(char* loc, char * command); //adds command to the end of the location i.e. "loc/command"
extern char *locationOfProg(char * relative); //Gets location of the shell


//*************************************************************
//********ARRAY HOLDING FORKED PROCESS INFO********************
//*************************************************************

//__________________STRUCTURES__________________________________
typedef struct processAr //Store for process ids in shared memory 
{
    int childrenArray[100];
    int childCount;
} childArray;

//__________________VARIABLES__________________________________

extern childArray * globalChildAr; //Holds all process IDs

//__________________FUNCTIONS__________________________________

extern void initializeProcessArray(childArray ** x); //Initializes it with shared memory so all children may modify it
extern void deleteProcessArray(childArray ** x); //deletes it
extern void addToArray(childArray ** x, int i); //Adds a process number to the array
extern void removeFromArray(childArray ** x, int i); //Removes a process number from the array




//*************************************************************
//********ARRAY LIST DATASTRUCTURE*****************************
//*************************************************************

//__________________STRUCTURES__________________________________

typedef struct node //Node of the array 
{
    char * path;
    struct node *ptr;
} NODE;

typedef struct List // Stores links of nodes with functions to modify all values
{
    NODE *front; //Start of the node
    NODE *temp; //Window/reference for access
    NODE **end; //Used for adding to the end of the array
} LIST;

//__________________FUNCTIONS__________________________________


extern void initializePathList(LIST ** pathlist, char * paths); //Initializes list with the path by breaking it apart
extern void initializeList(LIST **list); //Initializes an arrayList
extern void enqueue(LIST **list, char * paths); //Adds to the list
extern void delete(LIST **list); //Deletes all from the linked list (freed memory)
extern void resetHead(LIST **list); //Resets head to front
extern void next(LIST **list); //Next node

extern void display(LIST **list);//for debugging

//__________________VARIABLES__________________________________
extern LIST * pathList; //Holds a list of paths
extern LIST * cdList; //Holds a list of cdpaths


