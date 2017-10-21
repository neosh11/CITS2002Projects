#include "myshell.h"
#include "mystuff.h"

/*
   CITS2002 Project 2 2017
   Name(s):             student-name1 (, student-name2)
   Student number(s):   student-number-1 (, student-number-2)
   Date:                date-of-submission
 */

int shellInstance(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    //  INITIALIZE THE THREE INTERNAL VARIABLES
    printf("%s", FILELOCATION);
    HOME = getenv("HOME");
    if (HOME == NULL)
    {
        HOME = DEFAULT_HOME;
    }

    PATH = getenv("PATH");
    if (PATH == NULL)
    {
        PATH = DEFAULT_PATH;
    }

    CDPATH = getenv("CDPATH");
    if (CDPATH == NULL)
    {
        CDPATH = DEFAULT_CDPATH;
    }

    //STORE ALL PATH VARIABLES IN A LIST
    initializeList();
    char *valuePath = strtok(PATH, ":");
    enqueue(valuePath);
    
    while ((valuePath = strtok(NULL, ":")) != NULL)
    {
        enqueue(valuePath);
    }

    int statusShell = shellInstance(argc, argv);
    delete();

    return statusShell;
}

int shellInstance(int argc, char *argv[])
{
    //  REMEMBER THE PROGRAM'S NAME (TO REPORT ANY LATER ERROR MESSAGES)
    argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
    argc--; // skip 1st command-line argument
    
    //CANNOT SEE A USE FOR THIS AT THIS POINT
    argv++;

    //  DETERMINE IF THIS SHELL IS INTERACTIVE  //Maybe localize??
    interactive = (isatty(fileno(stdin)) && isatty(fileno(stdout)));

    int exitstatus = EXIT_SUCCESS;

    //  READ AND EXECUTE COMMANDS FROM stdin UNTIL IT IS CLOSED (with control-D)
    while (!feof(stdin))
    {
        SHELLCMD *t = parse_shellcmd(stdin);

        if (t != NULL)
        {

            //  WE COULD DISPLAY THE PARSED COMMAND-TREE, HERE, BY CALLING:
            //print_shellcmd(t);

            exitstatus = execute_shellcmd(t);

            free_shellcmd(t);
        }
    }
    if (interactive)
    {
        fputc('\n', stdout);
    }
    return exitstatus;
}
