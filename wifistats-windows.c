/*
   CITS2002 Project 1 2017
   Name(s):             Neosh Sheikh , Cameron Wright
   Student number(s):   21959462 , 21958958
   Date:                date-of-submission
*/


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAXMAC 1000
#define MAXSTR 100
#define MAXOUI 2500
#define MACLEN 18


//            LINKED-LIST
//*************************************
typedef struct node
{
    int total;
    char id[18];
    struct node *ptr;
} macNode;
macNode *front,*rear,*temp,*front1;
int count = 0;

void enq(int total, char * id);
void update(int total, char * id);
void display(void);
void create(void);
void delete(void);
//*************************************


bool findNameInFile(char id[9], FILE * file, char retStr[MAXSTR]);
void run(FILE *reference);
int part1(FILE *file, char type);
int part2(FILE *file, FILE *file2, char type);

int main(int argc, char *argv[])
{
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
        part1(file, type);
    }
    else if (argc == 4)
    {
        FILE *file2;
        file2 = fopen(argv[3], "r");

        if ((file2 = fopen(argv[3], "r")) == NULL)
        {
            //ERROR
        }
        part2(file, file2, type);
        fclose(file2);
    }
    fclose(file);

    //SUCCESS

    printf("\n%i", count);
    delete();
    
    return 0;
}

/***********************************************************

                Functions used in the program

************************************************************/
bool findNameInFile(char id[9], FILE * file, char retStr[MAXSTR])
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

void run(FILE *reference)
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
        found = findNameInFile(front1->id, reference, str);
        if(found)
        {
            printf("%s\t%s\t%d\n", front1->id, str, front1->total);
        }
       else
        {
            unknown += front1->total;
        }

        front1 = front1->ptr;
    }
    if (front1 == rear)
    {
        found = findNameInFile(front1->id, reference, str);
        if(found)
        {
            printf("%s\t%s\t%d\n", front1->id, str, front1->total);
        }
       else
        {
            unknown += front1->total;
        }
    }

    printf("??:??:??\tunknown\t%d\n", unknown);
}

int part1(FILE *file, char type)
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

    create();
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) != EOF)
    {
        update(len, useData);
    }

    display();
    return 0;
}

int part2(FILE *file, FILE *file2, char type)
{
    float time;
    char tData[MACLEN]; //Transmitter MAC
    char rData[MACLEN]; //Receiver MAC
    char *useData;      //Pointer to set up the data in use
    int len;            //len in bytes of packe

    char MAC[MAXMAC][MAXSTR];
    int totals[MAXMAC];

    int macCount = 0;
    int odiCount = 0;

    char buffer[500];
    int xMen = 0;
    
    if (type == 'r')
        useData = rData;
    else if (type == 't')
        useData = tData;
    else
        return -1;

    create();
    while (fscanf(file, "%f\t%s\t%s\t%i\n", &time, tData, rData, &len) != EOF)
    {
        char shor[9];
        strncpy(shor, useData, 8);
        shor[8] = '\0';

        update(len, shor);
    }
    run(file2);

    return 0;
}


/**********************************8
             LINKEDLIST
/***********************************/

void enq(int total, char * id)
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
    count++;

}
void update(int total, char * id){

    front1 = front;
    if ((front1 == NULL) && (rear == NULL))
    {
        enq(total, id);
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
    enq(total,id);

}
void display(void)
{

    front1 = front;
    
       if ((front1 == NULL) && (rear == NULL))
       {
           printf("Queue is empty");
           return;
       }
       while (front1 != rear)
       {
           printf("%s\t%d\n",front1->id, front1->total);
           front1 = front1->ptr;
       }
       if (front1 == rear)
       printf("%s\t%d\n",front1->id, front1->total);

}
void create(void)
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