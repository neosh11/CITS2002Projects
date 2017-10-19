#include "myshell.h"
#include "mystuff.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

int basicExecution(SHELLCMD *t);
int pathCommands(char * path, SHELLCMD *t);
int basicCommands(SHELLCMD *t);


//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    int exitstatus;

    if (t == NULL)
    { // hmmmm, that's a problem
        exitstatus = EXIT_FAILURE;
    }
    else
    {
        exitstatus = basicExecution(t);
    }
    return exitstatus;
}

int basicExecution(SHELLCMD *t)
{
    int status;

    if(strcmp(t->argv[0], "cd") == 0)
    {
        if(argc >1)
        {
            status = changeDirectory(argv[1]);
        }
        else
        {
            status = changeDirectory(NULL);
        }
    }
    else if(strchr(t->argv[0], '/') == NULL)
    {

        resetHead();
        while(temp != NULL)
        {
            char *location = locationCommand(temp->path, t->argv[0]);
            struct stat stat_buffer;

            if (stat(location, &stat_buffer) != 0)
            {
                next();
                continue;
            }

            status = pathCommands(temp->path, t);
            next();
            if(status == EXIT_SUCCESS)
                break;
        }            
    }
    else
    {
        status = basicCommands(t);
    }
    return status;
}
int pathCommands(char * path, SHELLCMD *t)
{
    int status;
    switch (fork())
    {

    case -1:
    {

        status = EXIT_FAILURE;
        break;
    }

    case 0:
    {
        
        char *location = locationCommand(path, t->argv[0]);
        struct stat stat_buffer;

        if (stat(location, &stat_buffer) != 0)
        {
            status = EXIT_FAILURE;
        }
        else if (S_ISREG(stat_buffer.st_mode))
        {
            execv(location, argumentsArray(t->argc, t->argv));
        }

        status = EXIT_SUCCESS;
        break;
    }
    default:
    {
        wait(&status);
        break;
    }
    }
    return status;
}

int basicCommands(SHELLCMD *t)
{
    int status;
    switch (fork())
    {

    case -1:
    {

        status = EXIT_FAILURE;
        break;
    }

    case 0:
    {
        
        char *location = t->argv[0];
        struct stat stat_buffer;

        if (stat(location, &stat_buffer) != 0)
        {
            status = EXIT_FAILURE;
        }
        else if (S_ISREG(stat_buffer.st_mode))
        {
            execv(location, argumentsArray(t->argc, t->argv));
        }

        status = EXIT_SUCCESS;
        break;
    }
    default:
    {
        wait(&status);
        break;
    }
    }
    return status;
}

int changeDirectory(char * dir)
{
    int status;
    if(dir != NULL)
    {
        status = chdir(dir);
    }
    else
    {
        status = chdir(HOME);
    }
    return status;
}