#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "StringQueue.h"
#include "StringArrQueue.h"
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "Stack.h"

#define MAX_ARGS_PER_CMD 12
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

int makeOperandsList(char *buf, char *tokens[]) {
    char *line;
    int i = 0;

    line = buf;
    tokens[i] = strtok(line, "<>|");
    printf("%d: %s\n", i, tokens[i]);
    do {
        i++;
        line = NULL;
        tokens[i] = strtok(line, "<>|");
            printf("%d: %s\n", i, tokens[i]);

    } while (tokens[i] != NULL);

    return i;
}
char** tokenizeOperandsString(char* buf) {
    char** tokens = malloc(sizeof(char*) * MAX_ARGS_PER_CMD);
    char* line = buf;
    int i = 0;
    tokens[i] = strtok(line, " \t\r\n");
    //printf(".%d.%s.\n", 0, tokens[0]);

    do {
        i++;
        line = NULL;
        tokens[i] = strtok(line, " \t\r\n");
        //printf(".%d.%s.\n",i,tokens[i]);
    } while (tokens[i] != NULL);

    return tokens;
}

int makeOperatorsList(char* buf, char* operator) {
    if (buf[0] == '\0') return 0;
    int i = 0;
    int j = 0;
    do {
        if (strchr("<>|", buf[i]) != NULL) {
            operator[j++] = buf[i];
        }
    } while (buf[++i] != '\0');
    return j;
}

char** extractTokens(char** tokens, int startInd, int endInd) {
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

void parseExpression(char** tokens, int n, StringQueue* operator, StringArrQueue* operand) {
    int startInd = 0, endInd = 0;
    int i = 0;

    while (i < n) {
        if (strstr("|<>", tokens[i]) != NULL) {
            endInd = i;
            enqueueStringArr(operand, extractTokens(tokens, startInd, endInd));
            enqueueString(operator, tokens[i]);
            startInd = endInd = i+1;
        } else {
            endInd++;
        }
        i++;
    }
    enqueueStringArr(operand, extractTokens(tokens, startInd, endInd));
}

int main(void) {
    StringQueue* q = initStringQueue(MAX_HISTORY /*100*/);

    while (true) {
        char input_line[MAX_CHARS];

        printf("Dustin> ");

        if (fgets(input_line, MAX_CHARS, stdin) != NULL) {
            enqueueString(q, input_line); // save command.  This can later be printed using the history command.

            if (strstr(input_line, "exit") == input_line) // exit the program
            {
                freeStringQueueData(q); // release data from memory
                //printf("Parent Process - Program exiting\n");
                exit(EXIT_SUCCESS);
            } else if (strstr(input_line, "history") == input_line) // print history
            {
                printf("History:\n");

                strtok(input_line, " \t\r\n");
                char* strNum = strtok(input_line, " \t\r\n");

                int len = 10;

                if (strNum != NULL) {
                    len = atoi(strNum);
                }

                (len > 0) ? printStringQueue(q, len) : printStringQueue(q, 10);
            } else // process commands
            {
                //printf("Processing command\n");

                pid_t shell1;

                int status;

                if (fork() == 0) { // create child process
                    // Create two queues, one for operators ( '|' '<' '>' ), and one for
                    // operands (grep WORD, cat something)
                    int numCmds, numOps;
                    char operator[15];
                    char *rawCommands[CMD_MAX];

                    numOps = makeOperatorsList(input_line, operator);
                    numCmds = makeOperandsList(input_line, rawCommands);

                    StringArrQueue* operand = initStringArrQueue(15);

                    for (int i = 0; i < numCmds; i++) {
                        char** tokOp = tokenizeOperandsString(rawCommands[i]);
                        enqueueStringArr(operand, tokOp);
                    }
                    printStringArrQueue(operand);

                    printf("numCmds: %d, numOps: %d\n", numCmds, numOps);
                    //for (int i = 0; i < numCmds; i++) {
                    //    printf("Token %d: %s\n", i, getStringArrElem(operand, i));
                    //}


                    //printf("Shell1 Initiated\n");



                    //parseExpression(tokens, n, operator, operand);

                    //int s = 0;
                    //while (tokens[s] != NULL) {
                    //    printf("Token %d: %s\n", s, tokens[s]);
                    //    s++;
                    //}

                    //if (operand->count > 0) {
                    //    printf(">>>>>>>>>>>> operands:\n");
                    //    printStringArrQueue(operand);
                    //}
                    //if (numOps > 0) {
                    //    printf(">>>>>>>>>>>> operators:\n");
                    //    printStringQueue(operator, 10);
                    //}

                    if (numOps == 0) {
                        // single argument --> execute statement;
                        char** cmds = dequeueStringArr(operand);
                        //printf("Executing command: %s\n", cmds[0]);
                        execvp(cmds[0], cmds);
                        printf("execvp(%s, [args]) FAILED.\n", cmds[0]);
                    } else {
                        if (operator[0] == '|') {
                            int numPipes = numOps;
                            int N = 2*numPipes;
                            //printf("exec '|' - %d\n", numPipes);

                            int* pipefd = (int*)malloc(sizeof(int) * numPipes * 2);

                            for (int i = 0; i < numPipes; i++) {
                                pipe(pipefd+i*2);
                            }

                            for (int i = 0; i < N+1; i += 2) {
                                if (fork() == 0) {
                                    if (i == 0) {
                                        //printf("start");
                                        dup2(pipefd[i+1], STDOUT_FILENO);

                                        for (int i = 0; i < N; i++)
                                            close(pipefd[i]);

                                        char** cmd = getStringArrElem(operand, i/2);
                                        //char* cmd[] = {"ls", NULL};
                                        execvp(cmd[0], cmd);
                                    } else if (i == N) {
                                        //printf("end");

                                        dup2(pipefd[i-2], STDIN_FILENO);

                                        for (int i = 0; i < N; i++)
                                            close(pipefd[i]);

                                        char** cmd = getStringArrElem(operand, i/2);
                                        //char* cmd[] = {"grep", "a", NULL};
                                        execvp(cmd[0], cmd);
                                    } else {
                                        dup2(pipefd[i-2], STDIN_FILENO);
                                        dup2(pipefd[i+1], STDOUT_FILENO);

                                        for (int i = 0; i < N; i++)
                                            close(pipefd[i]);

                                        char** cmd = getStringArrElem(operand, i/2);
                                        execvp(cmd[0], cmd);
                                    }
                                }
                            }

                            for (int i = 0; i < N; i++) {
                                close(pipefd[i]);
                            }

                            for (int i = 0; i < N; i++) {
                                wait(&status);
                            }
                        }
                        // redirection review: http://goo.gl/BnNEvC
                        else // argument is either '<' or '>'
                        {
                            /*
                            1. read operator
                            2. read two commands c0, c1
                            3. if '<', swap c0 and c1
                            4. if '>':
                            try open c0, if fails, dup, fork and execute

                            try open c1, if fails, dup, fork and execute
                            */

                            //printf("Processing IO Redirection\n");

                            int numPipes = numOps;
                            char** cmd;

                            for (int i = 0; i < numPipes; i++) {
                                char** cmd0 = getStringArrElem(operand, i);
                                char** cmd1 = getStringArrElem(operand, i+1);

                                if (operator[i] == '<') {
                                    int fd = open(cmd1[0], O_RDONLY);
                                    dup2(fd, STDIN_FILENO);
                                    close(fd);
                                    cmd = cmd0;
                                }
                                if (operator[i] == '>') {
                                    int fd = open(cmd1[0], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                                    dup2(fd, STDOUT_FILENO);
                                    close(fd);
                                    if (i == 0) {
                                        cmd = cmd0;
                                    }
                                }
                            }
                            if (fork() == 0) {
                                //printf("Executing: %s\n", cmd[0]);
                                execvp(cmd[0], cmd);
                            }
                            wait(NULL);
                        }
                    }

                    //printf("Child Process finished\n");
                    exit(EXIT_SUCCESS);

                } else { // parent process
                    // wait for the child process to terminate before providing the prompt
                    // for the user again.
                    wait(NULL);
                    //printf("Shell1 finished\n");

                }
            }
        } else {
            printf("Invalid Input\n");
        }
        wait(NULL);
    }
}

