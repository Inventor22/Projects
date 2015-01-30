#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "StringQueue.h"
#include "StringArrQueue.h"
#include <stdbool.h>

#include "Stack.h"

#define READ  0
#define WRITE 1

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
    char** args = malloc(sizeof(char*) * (endInd-startInd));
    int i = 0;
    for (; i < endInd-startInd; i++) {
        args[i] = tokens[startInd+i];
    }
    args[i] = NULL; // must add NULL terminator for exec() calls.
    return args;
}

void queueTest() {
    StringQueue* sq = initStringQueue(10);
    enqueueString(sq, "Hello");
    enqueueString(sq, "World");
    enqueueString(sq, "!!");
    printStringQueue(sq, 10);

    StringArrQueue* ssq = initStringArrQueue(10);
    char* a[] = {"one", "two", "three", NULL};
    char* b[] = {"four", "five", "six", NULL};

    char** d = a;

    char** e = malloc(3 * sizeof(char*));
    e[0] = "hello";
    e[1] = "world";
    e[2] = "3";
    e[3] = NULL;

    printf("dereference d: %s\n", d[0]);
    enqueueStringArr(ssq, e);
    enqueueStringArr(ssq, e);
    enqueueStringArr(ssq, b);

    while (ssq->count > 0) {
        char** c = dequeueStringArr(ssq);
        int i = 0;
        while (c[i] != NULL) {
            printf("element %d: %s\n", i, c[i]);
            i++;
        }
        printf("\n");
    }

}

int main(void) {
    //queueTest();
    //getchar();
    //exit(0);

    StringQueue* q = initStringQueue(MAX_HISTORY /*100*/);

    while (true) {
        char input_line[MAX_CHARS], *tokens[CMD_MAX];
        int n;

        printf("Dustin> ");
        if (fgets(input_line, MAX_CHARS, stdin) != NULL) {
            n = make_tokenlist(input_line, tokens);

            //printf("There are %d tokens.\n", n);
            //for (i = 0; i < n; i++)
            //    printf("extracted token is %s\n", tokens[i]);

            //for (int i = 0; i < strlen(tokens[0]); i++) {
            //    printf(".%c.\n", tokens[0][i]);
            //}

            //printf("strcmp: %d\n", strstr(tokens[0], "exit"));

            printf("n: %d\n", n);

            if (n == 1 && strcmp(tokens[0], "exit") == 0) // exit the program
            {
                freeStringQueueData(q); // release data from memory
                printf("Parent Process - Program exiting\n");
                exit(EXIT_SUCCESS);
            } else if ((n == 1 || n == 2) && strcmp(tokens[0], "history") == 0) // print history
            {
                printf("Printing History\n");
                if (n > 1) {
                    int len = atoi(tokens[1]);
                    if (len > 0) {
                        printStringQueue(q, len);
                    }
                } else {
                    printStringQueue(q, 10);
                }
            } else // process commands
            {
                printf("Processing command\n");
                enqueueString(q, input_line); // save command.  This can later be printed using the history command.
                //printStringQueue(q, 10);
                /*
                Do not need to create any pipes for shell1, since when fork() is called,
                all the information available to shell0 will be available to shell1 automatically.
                Once shell1 modifies the data, it no longer becomes shared between shell0 and shell1,
                and instead shell1 gets its own copy of the data.  Changes to data in shell1 will not
                affect data in shell0.

                Example solution for one of the things.

                Example solutions:
                sort < myFile
                1. pop operator from operator queue.
                2. pop 2 operands from operand queue
                3. open the file: int fd_in = open("myFile", O_READ) - use operand 2
                4. move standard input to the file: dup2(fd_in, FD_READ);
                5. execute sort command: execvp("sort", "sort", NULL); - use operand 1

                ls > myFile
                1. pop operator from operator queue
                2. pop 2 operands from operand queue
                3. Open file:
                int fd_out = open("myFile", O_WRITE) - use operand 2
                3. move standard output to file: dup2(fd_out, FD_WRITE);
                4. execute ls command: execvp("ls", "ls", NULL); - use operand 1

                sort < myFile > outFile
                1. pop operator from operator queue
                2. pop 2 operands from operand queue
                3. encounter '<':
                - open file from operand 2
                - dup2 input to myFile
                4. encounter '>':
                - open file from remaining operand queue
                - dup2 output to outFile

                Example solutions for the other thing:

                1. pop operators from operand queue
                2. pop 2 operands from operand queue
                3. create pipe, move output from operand 1 to pipe using dup2
                */
                pid_t shell1;

                if ((shell1 = fork()) == 0) { // create child process
                    // Create two queues, one for operators ( '|' '<' '>' ), and one for
                    // operands (grep WORD, cat something)

                    StringQueue*    operator = initStringQueue(5);
                    StringArrQueue* operand = initStringArrQueue(5);

                    printf("Shell1 Initiated\n");
                    int startInd = 0, endInd = 0;
                    int i = 0;

                    while (i < n) {
                        //printf("i: %d, s:%s\n", i, tokens[i]);
                        if (strcmp(tokens[i], "|") == 0 ||
                            strcmp(tokens[i], ">") == 0 ||
                            strcmp(tokens[i], "<") == 0) {
                            endInd = i;
                            enqueueStringArr(operand, getArgs(tokens, startInd, endInd));
                            enqueueString(operator, tokens[i]);
                            startInd = endInd = i+1;
                        } else {
                            endInd++;
                        }
                        i++;
                    }
                    //printf("out of while.  s: %d, e: %d\n", startInd, endInd);
                    enqueueStringArr(operand, getArgs(tokens, startInd, endInd));

                    if (operand->count > 0) {
                        printf(">>>>>>>>>>>> operands:\n");
                        printStringArrQueue(operand);
                    }
                    if (operator->count > 0) {
                        printf(">>>>>>>>>>>> operators:\n");
                        printStringQueue(operator, 10);
                    }

                    if (operator->count == 0) {
                        // single argument --> execute statement;
                        char** cmds = dequeueStringArr(operand);
                        printf("Executing command: %s\n", cmds[0]);

                        execvp(cmds[0], cmds);

                        printf("execvp(%s, [args] FAILED.\n", cmds[0]);
                    } else {
                        if (strcmp(topStringQueue(operator), "|") == 0) {

                            printf("in '|' command\n");


                            // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

                            int status;
                            int i = 0;
                            pid_t pid;

                            int* pipefds = (int*)malloc(sizeof(int) * operator->count * 2);
                            printf("pipes: %d\n", operator->count);
                            printf("commands: %d\n", operand->count);

                            for (i = 0; i < (operator->count); i++) {
                                if (pipe(pipefds + i*2) < 0) {
                                    perror("couldn't pipe");
                                    exit(EXIT_FAILURE);
                                }
                            }


                            int j = 0;
                            int x = 0;
                            int ind = 0;
                            while (x < operand->count) {
                                pid = fork();
                                if (pid == 0) {

                                    //if not last command
                                    if (x < operand->count-1) {
                                        if (dup2(pipefds[j + 1], 1) < 0) {
                                            perror("dup2");
                                            exit(EXIT_FAILURE);
                                        }
                                    }

                                    //if not first command&& j!= 2*numPipes
                                    if (j != 0) {
                                        if (dup2(pipefds[j-2], 0) < 0) {
                                           // if (dup2(pipefds[j], 0) < 0) {
                                            perror(" dup22");///j-2 0 j+1 1
                                            exit(EXIT_FAILURE);
                                        }
                                    }


                                    for (i = 0; i < 2*operator->count; i++) {
                                        close(pipefds[i]);
                                    }

                                    char** cmd = getStringArrElem(operand, ind);

                                    if (execvp(cmd[0], cmd) < 0) {
                                        perror(cmd[0]);
                                        exit(EXIT_FAILURE);
                                    }
                                } else if (pid < 0) {
                                    perror("error");
                                    exit(EXIT_FAILURE);
                                }

                                x++;
                                ind++;
                                j += 2;
                            }
                            /**Parent closes the pipes and wait for children*/

                            for (i = 0; i < 2 * operand->count; i++) {
                                close(pipefds[i]);
                            }

                            for (i = 0; i < operand->count + 1; i++)
                                wait(&status);

                            //int fd_incoming[2];
                            //int fd_outgoing[2];
                            //pid_t child;
                            //pipe(fd_outgoing);
                            ///*
                            //For first command:
                            //1. create pipe to second command
                            //2. fork.  Child -> setup , parent -> handle piping
                            //*/
                            //int ind = 0;

                            //if ((child = fork()) == 0)
                            //{
                            //    char** cmd = getStringArrElem(operand, ind);

                            //    printf("inside child 0.  Executing: ");
                            //    int x = 0;
                            //    while (cmd[x] != NULL) {
                            //        printf(" %s", cmd[x]);
                            //        x++;
                            //    }
                            //    printf(" DONE \n");

                            //    dup2(fd_outgoing[WRITE], STDOUT_FILENO);
                            //    close(fd_outgoing[WRITE]);
                            //    close(fd_outgoing[READ]);

                            //    execvp(cmd[0], cmd);

                            //    printf("Command failed.");
                            //    exit(EXIT_FAILURE);
                            //}
                            //else
                            //{
                            //    wait(NULL);
                            //    printf("child 0 finished\n");
                            //    fd_incoming[READ] = fd_outgoing[READ];
                            //    fd_incoming[WRITE] = fd_outgoing[WRITE];
                            //    ind++;
                            //}

                            //// process middle commands
                            //for (int i = 0; i < operand->count-2; i++)
                            //{
                            //    pipe(fd_outgoing);
                            //    if ((child = fork()) == 0)
                            //    {
                            //        printf("middle\n");

                            //        dup2(fd_incoming[READ], STDIN_FILENO);
                            //        dup2(fd_outgoing[WRITE], STDOUT_FILENO);
                            //        close(fd_incoming[READ]);
                            //        close(fd_incoming[WRITE]);
                            //        close(fd_outgoing[READ]);
                            //        close(fd_outgoing[WRITE]);

                            //        char** cmd = getStringArrElem(operand, ind);

                            //        execvp(cmd[0], cmd);

                            //        printf("Command failed.");
                            //        exit(EXIT_FAILURE);
                            //    }
                            //    else
                            //    {
                            //        wait(NULL);
                            //        close(fd_incoming[READ]);
                            //        close(fd_incoming[WRITE]);
                            //        fd_incoming[READ] = fd_outgoing[READ];
                            //        fd_incoming[WRITE] = fd_outgoing[WRITE];
                            //        ind++;
                            //    }
                            //}

                            //// process last command
                            //if ((child = fork()) == 0) {
                            //    char** cmd = getStringArrElem(operand, ind);

                            //    printf("in final command:");
                            //    int x = 0;
                            //    while (cmd[x] != NULL) {
                            //        printf(" %s", cmd[x]);
                            //        x++;
                            //    }
                            //    printf("\n");

                            //    dup2(fd_incoming[READ], STDIN_FILENO);
                            //    close(fd_incoming[READ]);
                            //    close(fd_incoming[WRITE]);

                            //    execvp(cmd[0], cmd);

                            //    printf("Command failed.");
                            //    exit(EXIT_FAILURE);
                            //} else {
                            //    printf("last command finished\n");
                            //    wait(NULL);
                            //    printf("last command finished\n");
                            //    close(fd_incoming[READ]);
                            //    close(fd_incoming[WRITE]);
                            //    close(fd_outgoing[READ]);
                            //    close(fd_outgoing[WRITE]);
                            //}

                            //if (child > 0) {
                            //    printf("Waiting on child: %d", pid(child));
                            //    wait(NULL);
                            //}

                        }
                        else // argument is either '<' or '>'
                        {
                            while (operator->count > 0) {
                                char* op = dequeueString(operator);
                                if (strcmp(op, "<") == 0) {

                                } else {

                                }
                            }
                        }
                    }

                    wait(NULL);
                    printf("Child Process finished\n");
                    exit(EXIT_SUCCESS);
                } else { // parent process
                    // wait for the child process to terminate before providing the prompt
                    // for the user again.
                    wait(NULL);
                    printf("Shell1 finished\n");

                }
            }
        } else {
            printf("Invalid Input\n");
        }
    }
}

