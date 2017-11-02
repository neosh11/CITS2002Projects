#include "mystuff.h"

/*
 CITS2002 Project 2 2017
 Names:             Neosh Sheikh, Cameron Wright
 Student number(s):   21959462, 21958958
 Date:                2/11/2017
 */
void initializeProcessArray(childArray ** x)
{
    *x = mmap(NULL, sizeof(childArray *), PROT_READ | PROT_WRITE, 
    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}
void deleteProcessArray(childArray ** x)
{
    munmap(*x, sizeof(childArray *));
}

void addToArray(childArray ** x, int i)
{
    (*x)->childrenArray[(*x)->childCount] = i;
    ((*x)->childCount)++;
}

void removeFromArray(childArray ** x, int i)
{
    int found = -1;
    for(int l = 0; l < (*x)->childCount; l++)
    {
        if(i ==  (*x)->childrenArray[l])
        {
            found = l;
            break;
        }
    }
    if(found != -1)
    {
        {
            for(int l = found; l < (*x)->childCount - 1; l++)
            {
                (*x)->childrenArray[l] =   (*x)->childrenArray[l+1];
            }
            (*x)->childrenArray[(*x)->childCount - 1] = -1;
        }
        (*x)->childCount--;
    }
}
