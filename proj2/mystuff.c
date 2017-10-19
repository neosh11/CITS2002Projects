#include "myshell.h"
#include "mystuff.h"

char **argumentsArray(int size,char **array)
{
	char **args = calloc(size, size*(sizeof(char*)));
	
	foreach(i, 1, size)
		args[i] = &array[i][0];
		
	return args;
}

char *locationCommand(char* loc, char * command)
{
	char *location = calloc(100, sizeof(char));
	strcpy(location, loc);
	strcat(location, command);
	return location;
}

