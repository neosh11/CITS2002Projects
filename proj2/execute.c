#include "execute.h"

/*
 CITS2002 Project 2 2017
 Names:             Neosh Sheikh, Cameron Wright
 Student number(s):   21959462, 21958958
 Date:                2/11/2017
 */
// -------------------------------------------------------------------

//Made global to get the exit status
int gloablexitstatus = 0;

//  THIS FUNCTION SHOULD TRAVERSE THE COMMAND-TREE and EXECUTE THE COMMANDS
//  THAT IT HOLDS, RETURNING THE APPROPRIATE EXIT-STATUS.
//  READ print_shellcmd0() IN globals.c TO SEE HOW TO TRAVERSE THE COMMAND-TREE

int execute_shellcmd(SHELLCMD *t)
{
    if (t == NULL)
    { // hmmmm, that's a problem
        gloablexitstatus = EXIT_FAILURE;
    }
    else
    {
        gloablexitstatus = categoryExecute(t);
    }
    return gloablexitstatus;
}


int categoryExecute(SHELLCMD *t)
{

    if (t == NULL)
    {
        return EXIT_SUCCESS;
    }

    int saved_stdout = dup(STDOUT_FILENO); //store stdout for restore
    int saved_stdin = dup(STDIN_FILENO);   //store stdin for restore
    bool failedIO = false;
    //The sequence   command < infile   requests that the command use infile as its standard input
    if (t->infile != NULL)
    {
        int fin = open(t->infile, O_RDONLY);

        if (fin == -1)
        {
            fprintf(stderr, "\nCould not open file %s\n\n", t->infile);
            failedIO = true;
        }
        if (!failedIO)
        {
            dup2(fin, STDIN_FILENO);
        }
        close(fin);
    }

    //The sequence   command >> outfile   requests that the command appends its standard output to the file outfile.
    //If outfile does not exist it is created.
    if (t->outfile != NULL && t->append)
    {
        int fap = open(t->outfile, O_WRONLY | O_APPEND | O_CREAT);
        if (fap == -1)
        {
            fprintf(stderr, "\nCould not open/create file %s\n\n", t->outfile);
            failedIO = true;
        }
        if (!failedIO)
        {
            dup2(fap, 1);
        }
        close(fap);
    }

    //The sequence   command > outfile   requests that the command use outfile as its standard output.
    //If the file outfile does not exist then it is created. If it does exist it is truncated to zero then rewritten.
    else if (t->outfile != NULL)
    {
        int fout = open(t->outfile, O_WRONLY | O_TRUNC | O_CREAT);

        if (fout == -1)
        {
            fprintf(stderr, "\nFailed to open/create file %s\n\n", t->outfile);
            failedIO = true;
        }
        if (!failedIO)
        {
            dup2(fout, 1);
        }
        close(fout);
    }

    int status = 1; //Status that will be returned, it's value is insignificant now

    if (!failedIO)
    {

        CMDTYPE typeCmd = t->type;

        if (typeCmd == CMD_COMMAND) //Executing commands (command nodes)
        {
            status = basicExecution(t);
        }

        else if (typeCmd == CMD_SEMICOLON) //Semicolon exec
        {
            status = categoryExecute(t->left);
            status = categoryExecute(t->right);
        }

        else if (typeCmd == CMD_AND) //And exec
        {
            status = categoryExecute(t->left);
            if (status == EXIT_SUCCESS)
            {
                status = categoryExecute(t->right);
            }
        }

        else if (typeCmd == CMD_OR) //Or exec
        {

            status = categoryExecute(t->left);

            if (status == EXIT_FAILURE)
            {
                status = categoryExecute(t->right);
            }
        }

        else if (typeCmd == CMD_SUBSHELL) //Subshell Exec
        {
            int tempChildId = fork();
            switch (tempChildId)
            {
            case -1:
                status = EXIT_FAILURE;
                break;
            case 0:
                exit(categoryExecute(t->left));
                break;
            default:
                addToArray(&globalChildAr, tempChildId);
                wait(&status);
                removeFromArray(&globalChildAr, tempChildId);
                break;
            }
        }

        else if (typeCmd == CMD_PIPE) //Pipe exec
        {
            int pipe1[2];
            bool failedPipe = false;
            if ((pipe(pipe1)) != 0)
            {
                failedPipe = true;
                fprintf(stderr, "\nFailed to pipe\n\n");
            }

            if (!failedPipe)
            {
                int tempChildId = fork();
                switch (tempChildId)
                {
                case -1:
                    fprintf(stderr,"\nfailed to fork\n\n");
                    status = EXIT_FAILURE;
                    break;
                case 0:
                    close(pipe1[0]);
                    dup2(pipe1[1], STDOUT_FILENO);
                    close(pipe1[1]);
                    exit(categoryExecute(t->left));
                    break;

                default:

                    addToArray(&globalChildAr, tempChildId);
                    wait(&status);
                    removeFromArray(&globalChildAr, tempChildId);
                    close(pipe1[1]);
                    dup2(pipe1[0], STDIN_FILENO);
                    close(pipe1[0]);
                    categoryExecute(t->right);

                    break;
                }
            }
            else
            {
                status = EXIT_FAILURE;
            }
            dup2(saved_stdout, STDOUT_FILENO);
            dup2(saved_stdin, STDIN_FILENO);
        }

        //TODO WHEN EXIT IS CALLED TERMINATE ALL PROCESSES
        else if (typeCmd == CMD_BACKGROUND) //EXEC IN BG
        {
            int tempChildId = fork();

            switch (tempChildId)
            {
            case -1:
                fprintf(stderr,"\nFailed to start the background processs\n\n");
                status = EXIT_FAILURE;
                break;
            case 0:

                categoryExecute(t->left);
                printf("\nFINISHED: ");
                print_shellcmd0(t->left);
                printf("\n");
                kill(getppid(), SIGUSR2); //Sends signal to parent so that parent waits for this process
                exit(0);
                break;

            default:
                addToArray(&globalChildAr, tempChildId);
                signal(SIGUSR2, killChild2);
                categoryExecute(t->right);
                status = EXIT_SUCCESS;
                fflush(stdout);
                break;
            }
        }
        else
        {
            //UH OH SOMETHING IS WRONG
            status = EXIT_FAILURE;
        }
    }
    else
    {
        status = EXIT_FAILURE;
    }

    dup2(saved_stdout, 1);
    dup2(saved_stdin, 0);

    close(saved_stdout); //Restore incase it was changed
    close(saved_stdin);  //Restore incase it was changed

    return status;
}

//***********************************

//Take Care of individual commands

int basicExecution(SHELLCMD *t)
{
    int status = EXIT_FAILURE;
    TIMEVAL startTime;
    TIMEVAL endTime;

    bool showTime = false;

    int cargc = t->argc;
    char **cargv = t->argv;

    if (strcmp(cargv[0], "time") == 0)
    {
        showTime = true;
        gettimeofday(&startTime, NULL);

        if (cargc == 1)
        {
            fprintf(stderr, "TIME TAKEN: %d\n", 0);
            status = EXIT_SUCCESS;
            return status;
        }
        else if (cargc > 1)
        {
            cargc -= 1;
            cargv = &cargv[1];
        }

        status = gloablexitstatus;
    }

    //TODO: UPDATE, IF DOESN'T COMMENCE WITH / CONSIDER CDPATH
    if (strcmp(cargv[0], "cd") == 0)
    {
        if (cargc > 1)
        {
            if (strchr(cargv[0], '/') == NULL)
            {
                resetHead(&cdList);
                while (cdList->temp != NULL)
                {
                    char *location = locationCommand(cdList->temp->path, cargv[1]);
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

                    if (status == EXIT_SUCCESS)
                        break;
                }
            }
        }
        else
        {
            status = changeDirectory(HOME);
        }

        if (status == EXIT_FAILURE)
        {
            fprintf(stderr, "\nNo such directory\n\n");
            return status;
        }
    }

    else if (strcmp(cargv[0], "exit") == 0)
    {
        if (cargc > 1)
        {
            exitCommand(atoi(cargv[1]));
        }
        else
        {
            exitCommand(gloablexitstatus);
        }
    }

    //CHECK IF IT DOES NOT CONTAIN A '/'
    //IF NOT, EXECUTE PATH COMMANDS i.e. execute from path if possible
    else if (strchr(cargv[0], '/') == NULL)
    {

        resetHead(&pathList);
        while (pathList->temp != NULL)
        {
            char *location = locationCommand(pathList->temp->path, cargv[0]);
            struct stat stat_buffer;

            if (stat(location, &stat_buffer) != 0)
            {
                next(&pathList);
                continue;
            }

            status = pathCommands(pathList->temp->path, t, showTime);
            free(location);

            next(&pathList);
            if (status == EXIT_SUCCESS)
                break;
        }
    }
    else
    {
        status = basicCommands(t, showTime);
        if (status != EXIT_SUCCESS)
        {
            fprintf(stderr, "\n%s: No such command\n\n", cargv[0]);
        }
    }

    //NEW SHELL TO RUN THE FILE

    //SHELL-SCRIPT
    if (status != EXIT_SUCCESS)
    {
        if (access(cargv[0], F_OK) != -1)
        {

            int tempChildId = fork();
            switch (tempChildId)
            {
            case -1:

                //TODO -check
                break;
            //failure

            case 0:
                //new copy of myshell
                //reads from a dup2 (file)
                {
                    if (cargv[0] != NULL)
                    {
                        int fin = open(cargv[0], O_RDONLY);

                        if (fin == -1)
                        {
                            //error
                        }

                        dup2(fin, 0);
                        close(fin);
                    }

                    execv(PROGLOCATION, (char *[]){"./shell", NULL});
                }
                break;
            //stuff

            default:
                addToArray(&globalChildAr, tempChildId);
                wait(&status);
                removeFromArray(&globalChildAr, tempChildId);
                break;
            }
        }
    }

    if (status != EXIT_SUCCESS)
    {
        fprintf(stderr, "\n%s: No such command\n\n", cargv[0]);
    }
    if (showTime)
    {
        gettimeofday(&endTime, NULL);
        uint64_t uStartTime = (startTime.tv_sec * (uint64_t)1000) + (startTime.tv_usec / 1000);
        uint64_t uEndTime = (endTime.tv_sec * (uint64_t)1000) + (endTime.tv_usec / 1000);
        fprintf(stderr, "\nTIME TAKEN: %" PRId64 "ms\n", uEndTime - uStartTime);
    }

    return status;
}

int pathCommands(char *path, SHELLCMD *t, bool showTime)
{

    int cargc = t->argc;
    char **cargv = t->argv;

    if (showTime)
    {
        cargc -= 1;
        cargv = &cargv[1];
    }

    int status;
    int tempChildId = fork();
    switch (tempChildId)
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
        addToArray(&globalChildAr, tempChildId);
        wait(&status);
        removeFromArray(&globalChildAr, tempChildId);
        break;
    }
    }

    return status;
}

int basicCommands(SHELLCMD *t, bool showTime)
{

    int cargc = t->argc;
    char **cargv = t->argv;

    if (showTime)
    {
        cargc -= 1;
        cargv = &cargv[1];
    }

    int status;

    int tempChildId = fork();
    switch (tempChildId)
    {

    case -1:
    {
        //TODOD
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
        addToArray(&globalChildAr, tempChildId);
        wait(&status);
        removeFromArray(&globalChildAr, tempChildId);
        break;
    }
    }
    return status;
}

int changeDirectory(char *dir)
{
    int status;
    if (dir != NULL)
    {
        status = chdir(dir);
    }
    else
    {
        status = chdir(HOME);
    }
    return status;
}

void exitCommand(int i)
{

    for(int i = 0; i < globalChildAr->childCount; i++)
    {
        kill(globalChildAr->childrenArray[i], SIGKILL);
    }

    delete(&pathList);
    delete(&cdList);
    deleteProcessArray(&globalChildAr);

    exit(i);
}


void killChild2(int sig)
{
    int x = wait(NULL);
    removeFromArray(&globalChildAr, x);
}

