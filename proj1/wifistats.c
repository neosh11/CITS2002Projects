/*
 CITS2002 Project 1 2017
 Name(s):             Neosh Sheikh , Cameron Wright
 Student number(s):   21959462 , 21958958
 Date:                22/09/2017
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

#define MAXSTR 91
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
    char id[MACLEN];
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



//************Part1 of the project************

/**
 * the function for part 1- the file is scanned and data extracted and functions are called to organize it
 * 
 * @param file the file to read from
 * @param type information on whether the file is using the transmitter or receiver MAC
 * @param stream the information to be displayed
 */
int part1(FILE *file, char type, FILE * stream);
/*************************************/



//************Part2 of the project************

/**
 * the function for part 2- the file is scanned and data extracted and functions are called to organize it
 * 
 * @param file the first file to read from
 * @param file2 the second file to read from
 * @param type information on whether the file is using the transmitter or receiver MAC
 * @param stream the information to be displayed
 */
int part2(FILE *file, FILE *file2, char type, FILE * stream);

/**
 * this method checks if the id given is in the file
 * 
 * @param id the id to be searched
 * @param file the file being read
 * @param retStr the string that is returned
 * @return true if the mac id was found, false otherwise
 */
bool findMacNameInFile(char id[9], FILE * file, char retStr[MAXSTR]);

/**
 * finds the data in the file and organizes it into the output
 * 
 * @param reference the data file containing references to vendors
 * @param stream file to be used
 */
void run(FILE *reference, FILE * stream);
/*************************************/

int main(int argc, char *argv[])
{
	
	//If incorrect number of arguments, exit
	if (argc != 3 && argc != 4)
	{
		return 0;
	}
			
    //Here the preprocessing is done in a child process and it's data is sent to the parent
    int pipes[2];
    pid_t pid = 0;
    FILE *stream;
    
    int stat;
    
    if(pipe(pipes) == -1)
        printf("could not create pipe\n");
    
    
    switch(fork()){
         
        case -1: //failure
            fprintf(stderr, "error forking\n");
            break;
            
        case 0: //In child 
            stream = fdopen(pipes[1], "w");
            
            if(stream == NULL)
                fprintf(stderr, "could not create file stream\n");
            
            if(close(pipes[0]) == -1)
                printf("error closing read end of pipe\n");
            
            char type = argv[1][0]; //First argument
            FILE *file;             //File to be opened
            
            
            if ((file = fopen(argv[2], "r")) == NULL)
            {
                fprintf(stderr, "error while opening file\n");
            }
            
			//If 2 arguments supplied, Run part 1
            if (argc == 3)
            {
                part1(file, type, stream);
            }
            
			//If 3 arguments supplied, Run part 2
            else if (argc == 4)
            {
                FILE *file2;
                file2 = fopen(argv[3], "r");
                
                if ((file2 = fopen(argv[3], "r")) == NULL)
                {
                	fprintf(stderr, "error while opening file\n");
                }
                part2(file, file2, type, stream);
                fclose(file2);
            }
			
            fclose(file);
            
            if(fclose(stream) == EOF)
                fprintf(stderr, "error while closing stream\n");
            break;
            
        default:
			//Wait for child to finish preprocessing
            wait(&stat);
            
			//Get input from child
            dup2(pipes[0], STDIN_FILENO);
            
            if(close(pipes[1]) == -1)
                fprintf(stderr,"error closing write end of pipe, pid = %d\n", pid);
            
            if(close(pipes[0]) == -1)
                fprintf(stderr,"error closing write end of pipe, pid = %d\n", pid);
			
			//Execute sorting depending on type
            if (argc == 3)
            {
                execv("/usr/bin/sort", (char*[]){"sort", "-k2,1nr", "-k1,2f", "-t", "\t", (char*)NULL});
            }
            
            else if (argc == 4)
            {
                execv("/usr/bin/sort", (char*[]){"sort", "-k3,1nr", "-k2,2f", "-t", "\t" , (char*)NULL});
            }
            break; 
    }
    
    //SUCCESS (Exit's both parent and child)
    return 0;
}
/*************************************/

/***********************************************************
 
 Functions used in the program
 
 ************************************************************/

bool findMacNameInFile(char id[9], FILE * file, char retStr[MAXSTR])
{
    //Search through files containing vendor names for name
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
    
    int unknown= 0;
    bool found;
    char str[MAXSTR];
    
    front1 = front;
    
    if ((front1 == NULL) && (rear == NULL))
    {
		//No elements = exit
        return;
    }
	
	//look at each element in list, if vendor name found print line else add totals of unknown
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
	//Add unknowns to stream
    if(unknown != 0)
        fprintf(stream, "??:??:??\tUNKNOWN-VENDOR\t%d\n", unknown);
}

int part1(FILE *file, char type, FILE *stream)
{
    float time;
    char tData[MACLEN]; //Transmitter MAC
    char rData[MACLEN]; //Receiver MAC
    char *useData;      //Pointer to set up the data in use
    int len;            //len in bytes of packe
    
    //choose the data to be used
    if (type == 'r')
        useData = rData;
    else if (type == 't')
        useData = tData;
    else
        return -1;
    
    initializeList();
	
	//Add all data into the linked list
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) == 4)
    {
		if(strcmp(tData,"ff:ff:ff:ff:ff:ff") != 0 && strcmp(rData,"ff:ff:ff:ff:ff:ff") != 0)
            update(len, useData);
    }
    
	//Put all data onto the stream
    display(stream);
	
	delete();
    return 0;
}

int part2(FILE *file, FILE *file2, char type, FILE *stream)
{
    
    float time;
    char tData[MACLEN]; //Transmitter MAC
    char rData[MACLEN]; //Receiver MAC
    char *useData;      //Pointer to set up the data in use
    int len;            //len in bytes of packe
    
	
	//Choose the data to be used
    if (type == 'r')
        useData = rData;
    else if (type == 't')
        useData = tData;
    else
        return -1;
    
    initializeList();
	
	//Add all data to linked list 
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) != EOF)
    {
        char shor[9];
        strncpy(shor, useData, 8);
        shor[8] = '\0';
        
        if(strcmp(tData,"ff:ff:ff:ff:ff:ff") != 0 && strcmp(rData,"ff:ff:ff:ff:ff:ff"))
            update(len, shor);
    }
	//Find vendor names
    run(file2, stream);
	
    delete();
    return 0;
}

//***********************************
//LINKEDLIST
//***********************************

/*
 * standard enqueue function for a linked list- adds an item to the back of the queue
 * 
 * @param total number of packets to be added to ID
 * @param id the mac id to add
 */
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

/*
 * adds total to name, if name already exists
 * 
 * @param total number of packets to be added to ID
 * @param id the mac id to add
 */
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

/*
 * adds all the nodes to the stream
 * @param stream the file where it's added
 */
void display(FILE * stream)
{
    
    front1 = front;
    
    if ((front1 == NULL) && (rear == NULL))
    {
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

/*
 * initialization of the linked list
 */
void initializeList(void)
{
    
    front = rear = NULL;
}

/*
 * deletes entire linked list to free memory
 */
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
        free(front1); //Free memory 
        front1 = temp;
    }
    if (front1 == rear)
    {
        free(front1);
    }
}
