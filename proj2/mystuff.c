#include "mystuff.h"

LIST * pathList; 
LIST * cdList;
char * PROGLOCATION;
childArray * globalChildAr;

/// GETS THE LOCATION OF THE ACTUAL PROGRAM

char *locationOfProg(char * relative)
{
    char * repath = calloc(PATH_MAX, sizeof(char));; 
    realpath(relative, repath);
    return repath; 
}

/// TAKES THE ARGV VECTOR AND RETURNS A VECTOR WITH JUST THE ARGUMENTS

char **argumentsArray(int size, char **array)
{
    char **args = calloc(size, size * (sizeof(char *)) + 1);

    foreach (i, 0, size)
        args[i] = &array[i][0];

    return args;
}

///  CONCATENTATES THE LOCATION AND THE COMMAND WITH A /  E.G. /usr/bin AS LOCATION AND ls AS COMMAND WILL RETURN /usr/bin/ls

char *locationCommand(char *loc, char *command)
{
    char *location = calloc(100, sizeof(char));
    strcpy(location, loc);
    strcat(location, "/");
    strcat(location, command);
    return location;
}

