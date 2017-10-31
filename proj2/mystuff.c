#include "myshell.h"
#include "mystuff.h"

LIST * pathList;
LIST * cdList;

char * PROGLOCATION;

//SYMLINK
char *locationOfProg(char * relative)
{
    char * repath = calloc(PATH_MAX, sizeof(char));; 
    realpath(relative, repath);
    return repath; 
}

//********************************

void initializeList(LIST **list)
{
    *list = (LIST *)malloc(sizeof(LIST));
    (*list)->front = NULL;
    (*list)->end = (NODE **)malloc(sizeof(NODE *));
}

//enques a node on to the list
void enqueue(LIST **list, char * paths)
{
    
    if ((*list)->front == NULL)
    {

        (*list)->front = (NODE *)malloc(sizeof(NODE));
        (*list)->front->path = (char *)strdup(paths);
        (*list)->front->ptr = NULL;
        (*list)->end = &((*list)->front->ptr);
    }
    else
    {
        (*(*list)->end) = (NODE *)malloc(sizeof(NODE));
        (*(*list)->end)->path = strdup(paths);
        
        NODE *x = (NODE *)malloc(sizeof(NODE));
        x = NULL;

        (*(*list)->end)->ptr = x;

        (*list)->end = &((*(*list)->end)->ptr);

    }
}

void display(LIST **list)
{
    (*list)->temp = (*list)->front;
    while ((*list)->temp != NULL)
    {
        printf("%s\n", (*list)->temp->path);
        (*list)->temp = (*list)->temp->ptr;
    }
}
void delete (LIST **list)
{
    while ((*list)->front != NULL)
    {
        (*list)->temp = (*list)->front->ptr;
        free((*list)->front->path);
        free((*list)->front);
        (*list)->front = (*list)->temp;
    }
    free((*list)->front);
    free((*list)->temp);
    free(*list);
}

void resetHead(LIST **list)
{
    (*list)->temp = (*list)->front;
}
void next(LIST **list)
{
    if ((*list)->temp != NULL)
    {
        (*list)->temp = (*list)->temp->ptr;
    }
}

//***************************************************

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
