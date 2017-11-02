#include "execute.h"

/*
 CITS2002 Project 2 2017
 Names:             Neosh Sheikh, Cameron Wright
 Student number(s):   21959462, 21958958
 Date:                2/11/2017
 */

int shellInstance(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    //  INITIALIZE THE THREE INTERNAL VARIABLES
    
    PROGLOCATION = locationOfProg(argv[0]);
    
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
    initializePathList(&pathList, PATH);
    //STORE ALL CDPATH VARIABLES IN A LIST
    initializePathList(&cdList, CDPATH);
    //Initialize global processId storage
    initializeProcessArray(&globalChildAr);

    //INITIALIZE SHELL
    int statusShell = shellInstance(argc, argv);
    
    exitCommand(1);

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



