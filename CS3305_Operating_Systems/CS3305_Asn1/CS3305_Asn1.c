#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "HistoryQueue.h"
#include <stdbool.h>

#define CMD_MAX 10
#define MAX_HISTORY 100

//Get experience with the fork(), execlp(), pipe(), and dup2(), system functions
/*
Useful resources:
Piping information from one command to the next: (Ex. a<b>c)
http://stackoverflow.com/questions/4812891/fork-and-pipes-in-c
*/

int make_tokenlist(char *buf, char *tokens[]) {
    char input_line[MAX_CHARS];
    char *line;
    int i, n;

    i = 0;

    line = buf;
    tokens[i] = strtok(line, " ");
    do {
        i++;
        line = NULL;
        tokens[i] = strtok(line, " ");
    } while (tokens[i] != NULL);

    return i;
}


int main(void) {

    HistoryQueue* q = initHistoryQueue(MAX_HISTORY);
    int quit = 0;

    while (true) {
        char input_line[MAX_CHARS], *tokens[CMD_MAX];
        int i, n;

        printf("Dustin> ");
        if (fgets(input_line, MAX_CHARS, stdin) != NULL)
        {
            n = make_tokenlist(input_line, tokens);

            for (i = 0; i < n; i++)
                printf("extracted token is %s\n", tokens[i]);

            if (strcmp(tokens[0], "exit") == 0)
            {
                printf("Child 0 - Program exiting\n");
                exit(EXIT_SUCCESS);
            }
            else if (strcmp(tokens[0], "history") == 0)
            {
                int len = atoi(tokens[1]);
                if (len != -1) {
                    printHistory(q, len);
                } else {
                    printHistory(q, 10);
                }
            }

        } else {
            printf("Invalid Input\n");
        }

        /*
        Do not need to create any pipes for shell1, since when fork() is called,\
        all the information available to shell0 will be available to shell1 automatically.
        Once shell1 modifies the data, it no longer becomes shared between shell0 and shell1,
        and instead shell1 gets it own copy of the data.  Changes to data in shell1 will not
        affect data in shell0.
        */
        pid_t shell1;

        if ((shell1 = fork()) == 0) { // create child process
            // parse input command, calling fork() and pipe() as needed

        } else { // parent process
            // wait for the child process to terminate before providing the prompt
            // for the user again.
            wait(NULL);
            printf("Child Process finished\n");
        }

    }

}

