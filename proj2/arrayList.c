#include "mystuff.h"

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

void initializePathList(LIST ** pathlist, char * paths)
{
    initializeList(pathlist);
    
    char *valuePath = malloc((strlen(paths)+1)*sizeof(char));
    char *PATHcopy = malloc((strlen(paths)+1)*sizeof(char));

    strcpy(PATHcopy, paths);
    valuePath = strtok(PATHcopy, ":");
    
    enqueue(pathlist, valuePath);
    
    while ((valuePath = strtok(NULL, ":")) != NULL)
    {
        enqueue(pathlist, valuePath);
    }
    free(valuePath);
    free(PATHcopy);
}

