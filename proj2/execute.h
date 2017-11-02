#include "myshell.h"
#include "mystuff.h"

/*
 CITS2002 Project 2 2017
 Names:             Neosh Sheikh, Cameron Wright
 Student number(s):   21959462, 21958958
 Date:                2/11/2017
 */

extern int gloablexitstatus;

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
 * Returns exit value
 * */
extern int categoryExecute(SHELLCMD *t);

/**
 * Responsible for checking if it's an internal command (cd, time, exit)
 * or if it's a system related command, either a path/external command
 * 
 * Returns exit value
 * */
extern int basicExecution(SHELLCMD *t); 

/**
 * Runs the path command by forking
 * returns exit value
 * */
extern int pathCommands(char * path, SHELLCMD *t, bool showTime); 

/**
 * Runs an external command by forking
 * returns exit value
 * */
extern int basicCommands(SHELLCMD *t, bool showTime);

/**
 * Changes directory, if it doesn't start with a '/' use CDPATH
 * else if null, HOME, else directory
 * */
extern int changeDirectory(char * dir);

/**
 * Exits the shell, frees memory allocated and kills all children processes
 * using a SIGKILL
 * */
extern void exitCommand(int i);

/**
 * Signal handler which waits for the child process to free process table
 * */ 
extern void killChild2(int sig);
