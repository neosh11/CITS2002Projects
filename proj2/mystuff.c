#include "myshell.h"
#include "mystuff.h"

void lala(int x)
{
	printfln("HI");
}

char **argumentsArray(int size,char * array[])
{
    char **args = calloc(size, size*(sizeof(char*))+1);
	foreach(i, 0, size)
		args[i] = &array[i][0];
		
	return args;
}