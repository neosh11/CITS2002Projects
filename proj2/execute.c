#include "myshell.h"
#include "mystuff.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

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
        if(strchr(t->argv[0], '/') == NULL)
        {

            resetHead();
            while(temp != NULL)
            {
                char *location = locationCommand(path, t->argv[0]);
                struct stat stat_buffer;

                if (stat(location, &stat_buffer) != 0)
                {
                    printf("%s doesn't exist", location);
                    continue;
                }

                exitstatus = pathCommands(temp->path, t);
                next();
                if(exitstatus == EXIT_SUCCESS)
                    break;
            }            
        }
        else
        {
            exitstatus = basicCommands(t);
        }

        
    }
    return exitstatus;
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
