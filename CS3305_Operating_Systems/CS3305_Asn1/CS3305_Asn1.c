#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "HistoryQueue.h"
#include <stdbool.h>



//Get experience with the fork(), execlp(), pipe(), and dup2(), system functions
/*
Useful resources:
Piping information from one command to the next: (Ex. a<b>c)
http://stackoverflow.com/questions/4812891/fork-and-pipes-in-c

Example using dup2 and fork:
http://stackoverflow.com/questions/3642732/using-dup2-for-piping

Example using redirection and pipes:
http://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html
*/

int make_tokenlist(char *buf, char *tokens[]) {
    char *line;
    int i;

    i = 0;

    line = buf;
    tokens[i] = strtok(line, " \r\n");
    do {
        i++;
        line = NULL;
        tokens[i] = strtok(line, " \r\n");
    } while (tokens[i] != NULL);

    return i;
}

char** getArgs(char** tokens, int startInd, int endInd) {
    char* args[CMD_MAX];
    for (int i = 0; i < endInd-startInd; i++) {
        args[i] = tokens[startInd+i];
    }
    return args;
}


int main(void) {
    //char* t = "exit";
    //printf("%d\n", strcmp(t, "exit"));

    HistoryQueue* q = initHistoryQueue(MAX_HISTORY /*100*/);

    while (true) {
        char input_line[MAX_CHARS], *tokens[CMD_MAX];
        int n;

        printf("Dustin> ");
        if (fgets(input_line, MAX_CHARS, stdin) != NULL)
        {
            n = make_tokenlist(input_line, tokens);

            //printf("There are %d tokens.\n", n);
            //for (i = 0; i < n; i++)
            //    printf("extracted token is %s\n", tokens[i]);

            //for (int i = 0; i < strlen(tokens[0]); i++) {
            //    printf(".%c.\n", tokens[0][i]);
            //}

            //printf("strcmp: %d\n", strstr(tokens[0], "exit"));

            //printf("n: %d\n", n);

            if (n == 1 && strcmp(tokens[0], "exit") == 0) // exit the program
            {
                freeHistoryQueueData(q); // release data from memory
                printf("Parent Process - Program exiting\n");
                exit(EXIT_SUCCESS);
            }
            else if ((n == 1 || n == 2) && strcmp(tokens[0], "history") == 0) // print history
            {
                printf("Printing History\n");
                if (n > 1) {
                    int len = atoi(tokens[1]);
                    if (len > 0) {
                        printHistory(q, len);
                    }
                } else {
                    printHistory(q, 10);
                }
            }
            else // process commands
            {
                printf("Processing command\n");
                enqueue(q, input_line); // save command.  This can later be printed using the history command.
                //printHistory(q, 10);
                /*
                Do not need to create any pipes for shell1, since when fork() is called,
                all the information available to shell0 will be available to shell1 automatically.
                Once shell1 modifies the data, it no longer becomes shared between shell0 and shell1,
                and instead shell1 gets its own copy of the data.  Changes to data in shell1 will not
                affect data in shell0.
                */
                pid_t shell1;

                if ((shell1 = fork()) == 0) { // create child process
                    // parse input command, calling fork() and pipe() as needed
                    // if a '|' command is encountered, use dup2 to redirect pipes, and 

                    bool isPipe = false;

                    printf("Shell1 Initiated\n");
                    int ind[4] = {0};
                    int i = 0;
                    while (i < n) {
                        if (strcmp(tokens[i], "|") == 0) {
                            ind[2] = i-1;
                            ind[3] = i+1;
                            while (true) {
                                if (tokens[i] == NULL) {
                                    ind[4] = i-1;

                                } else if (strcmp(tokens[i], "|") == 0) {
                                    ind[4] = i-1;
                                }
                            }
                        } else if (strcmp(tokens[i], "<") == 0) {
                            /*
                            redirect tokens from after tokens to before tokens.
                            before tokens will likely be a command, such as grep, ls, cat, etc.

                            Example:  grep FindThisWord < file
                            int fd_read = open("file", O_RDONLY);
                            dup2(fd_read, STDIN_FILENO);
                            execvp(tokens[0], tokens[0 to 1])

                            Example 2:  grep FindThisWord < input > output
                            int fd_read = open("input", O_RDONLY);
                            int fd_write = open("output", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                            dup2(fd_read, STDIN_FILENO);
                            dup2(fd_write, STDOUT_FILENO);


                            process:
                            1. store commands from before the token for execution.
                            */
                            ind[2] = i-1;

                            char* cmdArgs[10];

                            ind[3] = i+1;
                            while (true) {
                                if (tokens[0] != NULL) {}
                            }
                        } else if (strcmp(tokens[i], ">") == 0) {
                            /*
                            redirect output from before tokens to after tokens.
                            */
                        } else {

                        }
                        i++;
                    }

                    printf("Executing last command: %s", tokens[i-1]);


                    printf("Shell1 finished\n");
                    exit(EXIT_SUCCESS);
                } else { // parent process
                    // wait for the child process to terminate before providing the prompt
                    // for the user again.
                    wait(NULL);
                    printf("Child Process finished\n");
                }
            }
        } else {
            printf("Invalid Input\n");
        }
    }
}

