/*
 CITS2002 Project 1 2017
 Name(s):             Neosh Sheikh , Cameron Wright
 Student number(s):   21959462 , 21958958
 Date:                date-of-submission
 */

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAXMAC 500
#define MAXSTR 50
#define MAXOUI 2500
#define MACLEN 18


/************************************
*            LINKED-LIST            *
*************************************/

/*Struct representing a linked list
 *of Mac Ids and the number of packets
 *total: packets
 *id: ID associated
 *ptr: Location of the next node
 */

typedef struct node
{
    int total;
    char id[18];
    struct node *ptr;
} macNode;

//Identifier nodes for reference
macNode *front,*rear,*temp,*front1;


//initialize the list
void initializeList(void);


//enques a node on to the list
void enqueue(int total, char * id);


//adds total to name, if name already exists
void update(int total, char * id);


//Adds all the nodes to the stream
void display(FILE * stream);


//Frees used memory
void delete(void);
/*************************************/



//Part1 of the project
int part1(FILE *file, char type, FILE * stream);

//Part2 of the project
int part2(FILE *file, FILE *file2, char type, FILE * stream);
bool findMacNameInFile(char id[9], FILE * file, char retStr[MAXSTR]);
void run(FILE *reference, FILE * stream);




int main(int argc, char *argv[])
{
    
    int pipes[2];
    pid_t pid;
    FILE *stream;
    
    int stat;
    
    if(pipe(pipes) == -1)
        printf("could not create pipe\n");
    
    
    switch(fork()){
            
        case -1:
            fprintf(stderr, "error forking\n");
            break;
            
        case 0:
            stream = fdopen(pipes[1], "w");
            
            if(stream == NULL)
                fprintf(stderr, "could not create file streami\n");
            
            if(close(pipes[0]) == -1)
                printf("err closing read end pid\n");
            
            
            if (argc != 3 && argc != 4)
            {
                return 0;
            }
            
            char type = argv[1][0]; //Second argument
            FILE *file;             //File to be opened
            
            
            if ((file = fopen(argv[2], "r")) == NULL)
            {
                //ERROR
            }
            
            if (argc == 3)
            {
                part1(file, type, stream);
            }
            
            else if (argc == 4)
            {
                FILE *file2;
                file2 = fopen(argv[3], "r");
                
                if ((file2 = fopen(argv[3], "r")) == NULL)
                {
                    //ERROR
                }
                part2(file, file2, type, stream);
                fclose(file2);
            }
            fclose(file);
            
            if(fclose(stream) == EOF)
                fprintf(stderr, "error while closing stream\n");
            break;
            
        default:
            wait(&stat);
            
            dup2(pipes[0], STDIN_FILENO);
            
            if(close(pipes[1]) == -1)
                fprintf(stderr,"err closing write end pid=%d\n", pid);
            
            if(close(pipes[0]) == -1)
                fprintf(stderr,"err closing write end pid=%d\n", pid);
            if (argc == 3)
            {
                execv("/usr/bin/sort", (char*[]){"sort", "-k2,1nr", "-k1,2", "-t", "\t", (char*)NULL});
            }
            
            else if (argc == 4)
            {
                execv("/usr/bin/sort", (char*[]){"sort", "-k3,1nr", "-k2,2", "-t", "\t" , (char*)NULL});
            }
            exit(EXIT_FAILURE);
            
            break;
            
            
    }
    
    //SUCCESS
    return 0;
}


/***********************************************************
 
 Functions used in the program
 
 ************************************************************/
bool findMacNameInFile(char id[9], FILE * file, char retStr[MAXSTR])
{
    
    rewind(file);
    char currID[9];
    char currStr[MAXSTR];
    
    char buffer[500];
    while (fgets(buffer, 255, file))
    {
        for (int i = 0; i < 8; i++)
        {
            if (i == 2 || i == 5)
            {
                currID[i] = ':';
            }
            else
            {
                currID[i] = tolower((char)buffer[i]);
            }
        }
        currID[8] = '\0';
        if(strcmp(id, currID) == 0)
        {
            char *ab = &buffer[9];
            int x = 0;
            while (true)
            {
                if (ab[x] != '\n' && ab[x] != '\0')
                {
                    currStr[x] = ab[x];
                    x++;
                }
                else
                    break;
            }
            currStr[x] = '\0';
            strcpy(retStr, currStr);
            return true;
        }
    }
    return false;
}


void run(FILE *reference, FILE *stream)
{
    
    int unknown=0;
    bool found;
    char str[MAXSTR];

    front1 = front;
    if ((front1 == NULL) && (rear == NULL))
    {
        printf("Queue is empty");
        return;
    }
    while (front1 != rear)
    {
        found = findMacNameInFile(front1->id, reference, str);
        if(found)
        {
            fprintf(stream, "%s\t%s\t%d\n", front1->id, str, front1->total);
        }
        else
        {
            unknown += front1->total;
        }
        
        front1 = front1->ptr;
    }
    if (front1 == rear)
    {
        found = findMacNameInFile(front1->id, reference, str);
        if(found)
        {
            fprintf(stream, "%s\t%s\t%d\n", front1->id, str, front1->total);
        }
        else
        {
            unknown += front1->total;
        }
    }
    
    fprintf(stream, "??:??:??\tUNKNOWN-VENDOR\t%d\n", unknown);
}


int part1(FILE *file, char type, FILE *stream)
{
    float time;
    char tData[MACLEN]; //Transmitter MAC
    char rData[MACLEN]; //Receiver MAC
    char *useData;      //Pointer to set up the data in use
    int len;            //len in bytes of packe
    
    
    if (type == 'r')
        useData = rData;
    else if (type == 't')
        useData = tData;
    else
        return -1;
    
    initializeList();
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) != EOF)
    {
        update(len, useData);
    }
    
    display(stream);
    return 0;
}

int part2(FILE *file, FILE *file2, char type, FILE *stream)
{
    
    float time;
    char tData[MACLEN]; //Transmitter MAC
    char rData[MACLEN]; //Receiver MAC
    char *useData;      //Pointer to set up the data in use
    int len;            //len in bytes of packe
    
    char buffer[500];
    
    if (type == 'r')
        useData = rData;
    else if (type == 't')
        useData = tData;
    else
        return -1;
    
    initializeList();
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) != EOF)
    {
        char shor[9];
        strncpy(shor, useData, 8);
        shor[8] = '\0';
        
        update(len, shor);
    }
    run(file2, stream);

    
    return 0;
}

//***********************************
//LINKEDLIST
//***********************************

void enqueue(int total, char * id)
{
    if (rear == NULL)
    {
        rear = (struct node *)malloc(1*sizeof(struct node));
        rear->ptr = NULL;
        rear->total = total;
        strcpy(rear->id, id);
        front = rear;
    }
    else
    {
        temp=(struct node *)malloc(1*sizeof(struct node));
        rear->ptr = temp;
        temp->total = total;
        strcpy(temp->id,id);
        temp->ptr = NULL;
        rear = temp;
    }
    
}
void update(int total, char * id){
    
    front1 = front;
    if ((front1 == NULL) && (rear == NULL))
    {
        enqueue(total, id);
        return;
    }
    while (front1 != rear)
    {
        if(strcmp(id, front1->id) == 0)
        {
            front1->total += total;
            return;
        }
        front1 = front1->ptr;
    }
    if (front1 == rear){
        if(strcmp(id, front1->id) == 0)
        {
            front1->total += total;
            return;
        }
    }
    enqueue(total,id);
    
}
void display(FILE * stream)
{
    
    front1 = front;
    
    if ((front1 == NULL) && (rear == NULL))
    {
        fprintf(stream, "Queue is empty");
        return;
    }
    while (front1 != rear)
    {
        fprintf(stream, "%s\t%d\n",front1->id, front1->total);
        front1 = front1->ptr;
    }
    if (front1 == rear)
        fprintf(stream, "%s\t%d\n",front1->id, front1->total);
    
}
void initializeList(void)
{
    
    front = rear = NULL;
}


void delete(void)
{

    front1 = front;
    
       if ((front1 == NULL) && (rear == NULL))
       {
           return;
       }
       while (front1 != rear)
       {
           macNode * temp = front1->ptr;
           free(front1);
           front1 = temp;
       }
       if (front1 == rear)
       {
           free(front1);
       }
}
