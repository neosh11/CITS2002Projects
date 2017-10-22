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
int categoryExecute(SHELLCMD *t);
int changeDirectory(char * dir);

//Made global to get the exit status
int exitstatus=0;
bool showTime;


//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    
    if (t == NULL)
    { // hmmmm, that's a problem
        exitstatus = EXIT_FAILURE;
    }
    else
    {
        exitstatus = categoryExecute(t);
    }
    return exitstatus;
}

//CMD_COMMAND = 0,	// an actual command node itself
//CMD_SEMICOLON,		// as in   cmd1 ;  cmd2
//CMD_AND,		// as in   cmd1 && cmd2
//CMD_OR,			// as in   cmd1 || cmd2
//CMD_SUBSHELL,		// as in   ( cmds )
//CMD_PIPE,		// as in   cmd1 |  cmd2
//CMD_BACKGROUND

int categoryExecute(SHELLCMD *t)
{

     
    int saved_stdout = dup(1);
    int saved_stdin = dup(0);
    
    

    if(t->infile != NULL)
    {
        //printf("Going infile ");
        int fin = open(t->infile, O_RDONLY);
        
        if(fin == -1)
        {
            //error
        }
        
        dup2(fin, 0);
        close(fin);
    }
    else
    {
        //nothing
    }
    
    
    if(t->outfile != NULL && t->append)
    {
        //printf("Going outfile append");
        int fap = open(t->outfile, O_WRONLY| O_APPEND | O_CREAT);
        if(fap == -1)
        {
            //error
//            printf("\n%s\n", t->outfile);
//            printf("Failure\n");
        }
        dup2(fap, 1);
        close(fap);
        
    }
    else if(t->outfile != NULL)
    {
        //printf("Going outfile");
        int fout = open(t->outfile, O_WRONLY | O_TRUNC);
        
        if(fout == -1)
        {
            //   eroor
//            printf("\n%s\n", t->outfile);
//            printf("Failure\n");
        }
        dup2(fout, 1);
        close(fout);
        
    }
    else
    {
        //nothing
    }


    int status = 1;
    
    CMDTYPE typeCmd = t->type;
    
    if(typeCmd == CMD_COMMAND)
    {
        status = basicExecution(t);
    }
    else if(typeCmd == CMD_SEMICOLON)
    {
        status = categoryExecute(t->left);
        status = categoryExecute(t->right);
    }
    else if(typeCmd == CMD_AND)
    {
        status = categoryExecute(t->left);
        if(status == EXIT_SUCCESS)
        {
            status = categoryExecute(t->right);
        }
    }
    else if(typeCmd == CMD_OR)
    {
        
        status = categoryExecute(t->left);
        if(status == EXIT_FAILURE)
        {
            status = categoryExecute(t->right);
        }
    }
    else if(typeCmd == CMD_SUBSHELL)
    {
        
        switch(fork())
        {
            case -1:
                //failure
                break;
            case 0:
                exit( categoryExecute(t->left) );
                // exit(1);
                break;
            default:
                wait(&status);
        }
        
    }
    else if(typeCmd == CMD_PIPE)
    {
        int pipe1[2];
        
            if((pipe(pipe1)) != 0)
            {
                printf("FAIL");
            }
        
            switch(fork())
            {
            case -1:
        
                printf("FAIL");
            case 0:
                close(pipe1[0]);
                dup2(pipe1[1], 1);
                close(pipe1[1]);
                exit(categoryExecute(t->left));

            default:
                wait(&status);
                close(pipe1[1]);
                dup2(pipe1[0], 0);
                close(pipe1[0]);
                categoryExecute(t->right);
            }

            dup2(saved_stdout, 1);
            dup2(saved_stdin, 0);

    }
    else if(typeCmd == CMD_BACKGROUND)
    {
        
    }
    else
    {
        //UH OH SOMETHING IS WRONG
    }

    dup2(saved_stdout, 1);
    dup2(saved_stdin, 0);
    
    close(saved_stdout);
    close(saved_stdin);


    return status;
}


//***********************************

//Take Care of individual commands


int basicExecution(SHELLCMD *t)
{
    int status;
    TIMEVAL startTime;
    TIMEVAL endTime;
    
    showTime = false;
    
    int cargc = t->argc;
    char ** cargv = t->argv;
    

    
    if(strcmp(cargv[0], "time") == 0)
    {
        showTime = true;
        gettimeofday(&startTime, NULL);
        
        if(cargc == 1)
        {
            fprintf(stderr, "%d", 0);
            status = EXIT_SUCCESS;
            return status;
        }
        else if(cargc > 1)
        {
            cargc -= 1;
            cargv = &cargv[1];
        }
        status = exitstatus;
    }
    
    //TODO: Perhaps add other options
    if(strcmp(cargv[0], "cd") == 0)
    {
        if(cargc >1)
        {
            status = changeDirectory(cargv[1]);
        }
        else
        {
            status = changeDirectory(HOME);
        }
    }


    else if(strcmp(cargv[0], "exit") == 0)
    {
        if(cargc >1)
        {
            exit(atoi(cargv[1]));
        }
        else
        {
            exit(exitstatus);
        }
    }
    else if(strchr(cargv[0], '/') == NULL)
    {
        
        resetHead();
        while(temp != NULL)
        {
            char *location = locationCommand(temp->path, cargv[0]);
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

    printf("%d", status);

    if(status != EXIT_SUCCESS)
    {
        if (access(t->argv[0], F_OK) != -1)
        {
            printf("HEHE");
            status = EXIT_SUCCESS;

            switch(fork())
            {
            case -1:

            //failure

            case 0:
                //new copy of myshell
                //reads from a dup2 (file)
                {
                int fileO = open(t->argv[0], O_RDONLY);
                dup2(fileO, 0);
                close(fileO);
                execv(PROGLOCATION, (char*[]) {NULL})
                
                }

            //stuff

            default:
                wait(&status);
                if(status == -1)
                {
                    printf("%s", PROGLOCATION);
                }
            }
        }

    }
    
    if(showTime)
    {
        gettimeofday(&endTime, NULL);
        uint64_t uStartTime = (startTime.tv_sec * (uint64_t)1000) + (startTime.tv_usec / 1000);
        uint64_t uEndTime = (endTime.tv_sec * (uint64_t)1000) + (endTime.tv_usec / 1000);
        fprintf(stderr, "%llu\n", uEndTime-uStartTime);
    }

    return status;
}




int pathCommands(char * path, SHELLCMD *t)
{
    
    int cargc = t->argc;
    char ** cargv = t->argv;
    
    if(showTime)
    {
        cargc -= 1;
        cargv = &cargv[1];
    }
    
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
            
            char *location = locationCommand(path, cargv[0]);
            struct stat stat_buffer;
            
            if (stat(location, &stat_buffer) != 0)
            {
                status = EXIT_FAILURE;
            }
            else if (S_ISREG(stat_buffer.st_mode))
            {
                execv(location, argumentsArray(cargc, cargv));
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
    
    int cargc = t->argc;
    char ** cargv = t->argv;
    
    if(showTime)
    {
        cargc -= 1;
        cargv = &cargv[1];
    }
    
    
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
            
            char *location = cargv[0];
            struct stat stat_buffer;
            
            if (stat(location, &stat_buffer) != 0)
            {
                status = EXIT_FAILURE;
            }
            else if (S_ISREG(stat_buffer.st_mode))
            {
                execv(location, argumentsArray(cargc, cargv));
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
