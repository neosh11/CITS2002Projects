#include "myshell.h"
#include "mystuff.h"

PathList *front;
PathList *temp;

void initializeList(void)
{
    front = NULL;
}
//enques a node on to the list
void enqueue(char * paths)
{
    if(front == NULL)
    {
        PathList *x = (PathList*)malloc(sizeof(PathList));
        x = NULL;
        front = (PathList*)malloc(sizeof(PathList));
        front->path = (char*)strdup(paths);
        front->ptr = x;
    }
    else{
    PathList *x = (PathList*)malloc(sizeof(PathList));
    x->path = front->path;
    x->ptr = front->ptr;
    front->path = strdup(paths);
    front->ptr = x;
    }
}

void display(void)
{
    temp = front;
    while(temp != NULL)
    {
        printfln("%s", temp->path);
        temp = temp->ptr;
    }
}
void delete(void)
{
    while(front != NULL)
    {
        temp = front->ptr;
        free(front->path);
        free(front);
        front = temp;
    }
    free(front);
    free(temp);
}

char **argumentsArray(int size,char **array)
{
	char **args = calloc(size, size*(sizeof(char*))+1);
	
	foreach(i, 0, size)
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

