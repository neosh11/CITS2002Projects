#include "myshell.h"
#include "mystuff.h"

/*
   CITS2002 Project 2 2017
   Name(s):		student-name1 (, student-name2)
   Student number(s):	student-number-1 (, student-number-2)
   Date:		date-of-submission
 */

// -------------------------------------------------------------------

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
    { // normal, exit commands
        exitstatus = basicCommands(t);

    }

    return exitstatus;
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
        char *location1 = locationCommand("/bin/", t->argv[0]);
        char *location2 = locationCommand("/usr/bin/", t->argv[0]);

        struct stat stat_buffer1;
        struct stat stat_buffer2;

        if (stat(location1, &stat_buffer1) != 0)
        {
            if (stat(location2, &stat_buffer2) != 0)
            {
                status = EXIT_FAILURE;
            }
            else if (S_ISREG(stat_buffer2.st_mode))
            {
                execv(location2, argumentsArray(t->argc, t->argv));
            }
        }
        else if (S_ISREG(stat_buffer1.st_mode))
        {
            execv(location1, argumentsArray(t->argc, t->argv));
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
