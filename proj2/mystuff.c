#include "mystuff.h"

LIST * pathList; 
LIST * cdList;
char * PROGLOCATION;
childArray * globalChildAr;

//Gets the location of the actual program
char *locationOfProg(char * relative)
{
    char * repath = calloc(PATH_MAX, sizeof(char));; 
    realpath(relative, repath);
    return repath; 
}


char **argumentsArray(int size, char **array)
{
    char **args = calloc(size, size * (sizeof(char *)) + 1);

    foreach (i, 0, size)
        args[i] = &array[i][0];

    return args;
}

char *locationCommand(char *loc, char *command)
{
    char *location = calloc(100, sizeof(char));
    strcpy(location, loc);
    strcat(location, "/");
    strcat(location, command);
    return location;
}
