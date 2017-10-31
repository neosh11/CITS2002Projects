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
/**
 *This function traverses through the binary tree and acts appropriately depending on
 *the node it's on.
 *It is responsible for:
 * 1)Input/Output redirection
 *
 * 2)Executing commands (command nodes)
 * 3)Semicolon exec as in   cmd1 ;  cmd2
 * 4)And exec as in   cmd1 && cmd2
 * 5)Or exec as in   cmd1 || cmd2
 * 6)Subshell exec as in   (cmds)
 * 7)Pipe exec as in   cmd1 | cmd2
 * 8)Background exec
 *
 *
 * */

int categoryExecute(SHELLCMD *t)
{

     
    int saved_stdout = dup(STDOUT_FILENO);//store stdout for restore
    int saved_stdin = dup(STDIN_FILENO);//store stdin for restore
    
    
    //The sequence   command < infile   requests that the command use infile as its standard input
    if(t->infile != NULL)
    {
        int fin = open(t->infile, O_RDONLY);
        
        if(fin == -1)
        {
            //error TODO
            //couldn't open file
        }
        
        dup2(fin, STDIN_FILENO);
        close(fin);
    }
    
    //The sequence   command >> outfile   requests that the command appends its standard output to the file outfile.
    //If outfile does not exist it is created.
    if(t->outfile != NULL && t->append)
    {
        int fap = open(t->outfile, O_WRONLY| O_APPEND | O_CREAT);
        if(fap == -1)
        {
            //error TODO
            //Couldn't create/write to file
        }
        dup2(fap, 1);
        close(fap);
        
    }
    //The sequence   command > outfile   requests that the command use outfile as its standard output.
    //If the file outfile does not exist then it is created. If it does exist it is truncated to zero then rewritten.

    
    else if(t->outfile != NULL)
    {
        int fout = open(t->outfile, O_WRONLY | O_TRUNC | O_CREAT);
        
        if(fout == -1)
        {
            //   error TODO
            //Couldn't create/write to file
        }
        dup2(fout, 1);
        close(fout);
        
    }



    int status = 1;  //Status that will be returned, it's value is insignificant now
    
    CMDTYPE typeCmd = t->type;
    
    if(typeCmd == CMD_COMMAND) //Executing commands (command nodes)
    {
        status = basicExecution(t);
    }
    
    
    else if(typeCmd == CMD_SEMICOLON) //Semicolon exec
    {
        status = categoryExecute(t->left);
        status = categoryExecute(t->right);
    }
    
    
    else if(typeCmd == CMD_AND) //And exec
    {
        status = categoryExecute(t->left);
        if(status == EXIT_SUCCESS)
        {
            status = categoryExecute(t->right);
        }
    }
    
    
    else if(typeCmd == CMD_OR) //Or exec
    {
        
        status = categoryExecute(t->left);
        if(status == EXIT_FAILURE)
        {
            status = categoryExecute(t->right);
        }
    }
    
    
    else if(typeCmd == CMD_SUBSHELL) //Subshell Exec
    {
        
        switch(fork())
        {
            case -1:
                //failure TODO
                break;
            case 0:
                exit( categoryExecute(t->left) );
                break;
            default:
                wait(&status);
        }
        
    }
    
    
    else if(typeCmd == CMD_PIPE)  //Pipe exec
    {
        int pipe1[2];
        
            if((pipe(pipe1)) != 0)
            {
                //TODO
                printf("FAIL");
            }
        
            switch(fork())
            {
            case -1:
                //TODO
                printf("FAIL");
            case 0:
                close(pipe1[0]);
                dup2(pipe1[1], STDOUT_FILENO);
                close(pipe1[1]);
                exit(categoryExecute(t->left));

            default:
                wait(&status);
                close(pipe1[1]);
                dup2(pipe1[0], STDIN_FILENO);
                close(pipe1[0]);
                categoryExecute(t->right);
            }

            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stdin, STDIN_FILENO);

    }
    
    
    else if(typeCmd == CMD_BACKGROUND) //EXEC IN BG
    {
        
    }
    else
    {
        //UH OH SOMETHING IS WRONG
    }

    dup2(saved_stdout, 1);
    dup2(saved_stdin, 0);
    
    close(saved_stdout); //Restore incase it was changed
    close(saved_stdin); //Restore incase it was changed


    printf("%d", status);
    
    return status;
}


//***********************************

//Take Care of individual commands


int basicExecution(SHELLCMD *t)
{
    int status = EXIT_FAILURE;
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
    
    //TODO: UPDATE, IF DOESN'T COMMENCE WITH / CONSIDER CDPATH
    if(strcmp(cargv[0], "cd") == 0)
    {
        if(cargc >1)
        {
            if(strchr(cargv[0], '/') == NULL)
            {
                resetHead(&cdList);
                while(cdList->temp != NULL)
                {
                    char *location = locationCommand(cdList->temp->path, cargv[1]);
                    printf("%s", location);
                    struct stat stat_buffer;
                    
                    if (stat(location, &stat_buffer) != 0)
                    {
                        next(&cdList);
                        free(location);
                        continue;
                    }

                    status = changeDirectory(location);
                    free(location);
                    
                    next(&cdList);
                    
                    if(status == EXIT_SUCCESS)
                        break;
                }

            }
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
    
    //CHECK IF IT DOES NOT CONTAIN A '/'
    //IF NOT, EXECUTE PATH COMMANDS i.e. execute from path if possible
    else if(strchr(cargv[0], '/') == NULL)
    {
        
        resetHead(&pathList);
        while(pathList->temp != NULL)
        {
            char *location = locationCommand(pathList->temp->path, cargv[0]);
            struct stat stat_buffer;
            
            if (stat(location, &stat_buffer) != 0)
            {
                next(&pathList);
                continue;
            }
            
            status = pathCommands(pathList->temp->path, t);
            free(location);
            
            next(&pathList);
            if(status == EXIT_SUCCESS)
                break;
        }
    }
    else
    {
        status = basicCommands(t);
    }
    
    //NEW SHELL TO RUN THE FILE
    
    

    if(status != EXIT_SUCCESS)
    {
        if (access(t->argv[0], F_OK) != -1)
        {
            printf("HEHE");
            
            switch(fork())
            {
            case -1:

            //failure

            case 0:
                //new copy of myshell
                //reads from a dup2 (file)
                {
                    if(t->argv[0] != NULL)
                    {
                        int fin = open(t->argv[0], O_RDONLY);
                        
                        if(fin == -1)
                        {
                            //error
                        }
                        
                        dup2(fin, 0);
                        close(fin);
                    }
                    
                    execv(PROGLOCATION, (char*[]) {"./shell" ,NULL});
                    
                }
            //stuff

            default:
                wait(&status);
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
                exit(status);
            }
            else if (S_ISREG(stat_buffer.st_mode))
            {
                status = execv(location, argumentsArray(cargc, cargv));
            }
            else
            {
                status = EXIT_FAILURE;
                exit(status);
            }

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
            

            status = 0;
            if (stat(location, &stat_buffer) != 0)
            {
                status = EXIT_FAILURE;
                exit(status);
            }
            else if (S_ISREG(stat_buffer.st_mode))
            {
                status = execv(location, argumentsArray(cargc, cargv));
            }
            else
            {
                status = EXIT_FAILURE;
                exit(status);
            }

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
