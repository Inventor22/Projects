/*
File:       CS3305_Asn1.c
Date:       01/02/15
Name:       Dustin Dobransky
Student ID: 250575030
Alias:      ddobran

Purpose:
    Get experience with the fork(), execvp(), pipe(), and dup2() system functions

Description:
    This program emulates a simple bash shell.
    Supported command types fall into two categories:
        1. Redirection: ls > file, sort < file > newFile, etc.
           - only one of each of '<' and '>' will be present in a command

        2. Piping: ls -a | grep a | grep b | grep c
           - a maximum of 3 pipes need to be supported

    Only one command type needs to be supported in a single statement.

Notes:
    - Token code adapted from assignment resources
    - Once I got 'middle' pipes working, i.e. for a command like: "ls -a | grep a | grep b"
      where "grep a" would be the middle command, it was easy to extend functionality to
      support a larger number of pipes by counting the number of operators ('<', '>', '|'),
      and using a for loop.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "StringQueue.h"
#include "StringArrQueue.h"

#include "ShellConstants.h"

/*
Tokenize entire command.
example: buf = "ls -a | grep a | grep b|grep c
becomes: tokens = ["ls -a", "grep a", "grep b", "grep c", NULL]
*/
int makeOperandsList(char *buf, char *tokens[])
{
    char *line;
    int i = 0;
    line = buf;
    tokens[i] = strtok(line, "<>|");
    do
    {
        i++;
        line = NULL;
        tokens[i] = strtok(line, "<>|");
    } while (tokens[i] != NULL);

    return i;
}

/*
Tokenizes single command.
example:    buf = "ls -a"
becomes: tokens = ["ls", "-a", NULL]
*/
char** tokenizeOperandsString(char* buf)
{
    char** tokens = malloc(sizeof(char*) * MAX_ARGS_PER_CMD);
    char* line = buf;
    int i = 0;
    tokens[i] = strtok(line, " \t\r\n");

    do
    {
        i++;
        line = NULL;
        tokens[i] = strtok(line, " \t\r\n");
    } while (tokens[i] != NULL);

    return tokens;
}

/*
Creates list of operators, ordered by appearance
Example: "sort < file > newFile"
Becomes: operator = ['<', '>']
*/
int makeOperatorsList(char* buf, char* operator)
{
    if (buf[0] == '\0') return 0;
    int i = 0;
    int j = 0;
    do
    {
        if (strchr("<>|", buf[i]) != NULL)
        {
            operator[j++] = buf[i];
        }
    } while (buf[++i] != '\0');
    return j;
}

/*
Fetch and execute next command
*/
void executeNextCommand(StringArrQueue* operand, int i)
{
    char** cmd = getStringArrElem(operand, i/2);
    execvp(cmd[0], cmd);

    printf("'%s' has FAILED\n", cmd[0]);
    exit(EXIT_FAILURE);
}

int main(void)
{
    // stores all previously recorded commands, including calls to the 'history' command
    StringQueue* q = initStringQueue(MAX_HISTORY /*100*/);

    while (true)
    {
        char input_line[MAX_CHARS];

        printf("Dustin> ");

        if (fgets(input_line, MAX_CHARS, stdin) != NULL)
        {
            enqueueString(q, input_line); // save command.  Commands can later be printed using the history command.

            // success only if 'exit' is first word.  Success: "exit 12345", Failure: "sort < file.txt > exit.txt"
            if (strstr(input_line, "exit") == input_line) // exit the program
            {
                freeStringQueueData(q); // release data from memory
                free(q);

                exit(EXIT_SUCCESS);
            }
            else if (strstr(input_line, "history") == input_line) // print history. Syntax: "history" OR "history <num>"
            {
                printf("History:\n");

                strtok(input_line, " \t\r\n"); // throwaway
                char* strNum = strtok(NULL, " \t\r\n");

                int len = 10;

                if (strNum != NULL)
                    len = atoi(strNum);

                if (len > 0) printStringQueue(q, len);
                else         printStringQueue(q, 10); // print history
            }
            else // process commands
            {
                int status;

                pid_t shell1 = fork(); // create child process

                if (shell1 == -1)
                {
                    printf("Failed to create process.\n");
                }
                else if (shell1 == 0)
                {
                    int numCmds, numOps;
                    char operator[15]; // stores { '<', '>', '|' }
                    char *rawCommands[CMD_MAX /*16*/];

                    numOps = makeOperatorsList(input_line, operator);
                    numCmds = makeOperandsList(input_line, rawCommands);

                    // holds lists of operands.  Example:  "ls -la | grep a | grep b"
                    // becomes:  operand = [["ls", "-la", NULL], ["grep", "a", NULL], ["grep", "b", NULL]]
                    StringArrQueue* operand = initStringArrQueue(15);

                    for (int i = 0; i < numCmds; i++)
                    {
                        // example: "ls -a" --> becomes: ["ls", "-a", NULL]
                        char** tokOp = tokenizeOperandsString(rawCommands[i]);
                        enqueueStringArr(operand, tokOp);
                    }

                    if (numOps == 0)
                    {
                        // single argument --> execute statement;
                        char** cmds = dequeueStringArr(operand);
                        execvp(cmds[0], cmds);
                        printf("execvp(%s, ...) FAILED.\n", cmds[0]);
                    }
                    else
                    {
                        if (operator[0] == '|')
                        {
                            int N = 2*numOps; // each pipe has read and write ends

                            // allocate memory for necessary number of pipe file descriptors
                            int* pipefd = (int*)malloc(sizeof(int) * numOps * 2);

                            // create all pipes
                            for (int i = 0; i < numOps; i++)
                                pipe(pipefd+i*2);

                            for (int i = 0; i < N+1; i += 2)
                            {
                                pid_t child = fork();
                                if (child < 0)
                                {
                                    printf("Failed to create process.  Terminating.\n");
                                    exit(EXIT_FAILURE);
                                }
                                else if (child == 0)
                                {
                                    if (i == 0) // first command --> no input redirection
                                    {
                                        // redirect standard output (write) to current pipe
                                        dup2(pipefd[i+1], STDOUT_FILENO);

                                        for (int i = 0; i < N; i++) // close all pipes on child end
                                            close(pipefd[i]);

                                        executeNextCommand(operand, i);
                                    }
                                    else if (i == N) // last command --> no output redirection
                                    {
                                        // redirect standard input (read) to previous pipe
                                        dup2(pipefd[i-2], STDIN_FILENO);

                                        for (int i = 0; i < N; i++)
                                            close(pipefd[i]);

                                        executeNextCommand(operand, i);
                                    }
                                    else // middle commands --> redirect input and output
                                    {
                                        // redirect standard input (read) to previous pipe
                                        dup2(pipefd[i-2], STDIN_FILENO);
                                        // redirect standard output (write) to current pipe
                                        dup2(pipefd[i+1], STDOUT_FILENO);

                                        for (int i = 0; i < N; i++)
                                            close(pipefd[i]);

                                        executeNextCommand(operand, i);
                                    }
                                }
                            }

                            for (int i = 0; i < N; i++) // close all pipes on parent end
                                close(pipefd[i]);

                            for (int i = 0; i < N; i++) // wait for all children to terminate
                                wait(&status);

                            free(pipefd); // free malloc'd pipe memory
                        }
                        else // argument is either '<' or '>'
                        {
                            // redirection review: http://goo.gl/BnNEvC
                            char** cmd;

                            for (int i = 0; i < numOps; i++)
                            {
                                char** cmd0 = getStringArrElem(operand, i);
                                char** cmd1 = getStringArrElem(operand, i+1);

                                if (operator[i] == '<')
                                {
                                    int fd = open(cmd1[0], O_RDONLY); // open file to read
                                    dup2(fd, STDIN_FILENO); // redirect standard input to the file
                                    close(fd); // close off file descriptor
                                    cmd = cmd0; // assign command
                                }
                                if (operator[i] == '>')
                                {
                                    // open file for write
                                    int fd = open(cmd1[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                    dup2(fd, STDOUT_FILENO); // redirect standard output to the file
                                    close(fd);
                                    // support for commands: "ls > file"
                                    // prevents errors on commands: "sort < file > sortedFile"
                                    if (i == 0)
                                        cmd = cmd0;
                                }
                            }

                            pid_t child = fork();
                            if (child < 0)
                            {
                                printf("Failed to create process. Terminating.\n");
                                exit(EXIT_FAILURE);
                            }
                            else if (child == 0)
                            {
                                execvp(cmd[0], cmd);

                                printf("Failed to execute '%s'\n", cmd[0]);
                                exit(EXIT_FAILURE);
                            }

                            wait(NULL); // parent waits for child process to complete
                        }
                    }

                    freeStringArrQueueData(operand); // free memory
                    free(operand);

                    exit(EXIT_SUCCESS);
                }
                else // parent process
                {
                    // wait for the child process (shell1) to terminate before providing the prompt
                    // for the user again.
                    wait(NULL);
                }
            }
        }
        else
        {
            printf("Invalid Input\n");
        }
    }
}

