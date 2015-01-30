/*
      The following program creates a pipe, and then fork(2)s to create a
      child process; the child inherits a duplicate set of file descriptors
      that refer to the same pipe.  After the fork(2), each process closes
      the descriptors that it doesn't need for the pipe (see pipe(7)).  The
      parent then writes the string contained in the program's command-line
      argument to the pipe, and the child reads this string a byte at a
      time from the pipe and echoes it on standard output.
      */
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define R 0
#define W 1

int main(int argc, char *argv[])
{
    //int pipefd[6];
    //pid_t cpid;
    //int status;
    char* cmd[] = {"ls", NULL};
    char* cmdd[] = {"grep", "a", NULL};
    char* cmddd[] = {"grep", "R", NULL};


    //if (fork() == 0) {
    //    for (int i = 0; i < 2*2; i+=2) {
    //        pipe(pipefd+i);

    //        if (fork() == 0) {
    //            if (i == 0) { // first
    //                //printf("first\n");
    //                close(pipefd[i]); // read
    //                //printf("child closed read %d\n", i/2);
    //                dup2(pipefd[i+1], STDOUT_FILENO);

    //               /* for (int i = 0; i < 4; i++) {
    //                    close(pipefd[i]);
    //                }*/

    //                execvp(cmd[0], cmd);
    //            } else if (i == 2) { // last
    //                //printf("last\n");
    //                close(pipefd[i-2]); // write
    //                //printf("child closed write %d\n", (i/2));
    //                dup2(pipefd[i], STDIN_FILENO);

    //                //for (int i = 0; i < 4; i++) {
    //                //    close(pipefd[i]);
    //                //}

    //                execvp(cmdd[0], cmdd);
    //            } else { //middle
    //                //printf("middle\n");
    //                close(pipefd[i-2]);
    //                close(pipefd[i+1]);
    //                //printf("child closed write %d\n", (i/2));
    //                //printf("child closed read %d\n", (i/2)-1);
    //                dup2(pipefd[i-1], STDOUT_FILENO);
    //                dup2(pipefd[i], STDIN_FILENO);

    //                //for (int i = 0; i < 4; i++) {
    //                //    close(pipefd[i]);
    //                //}

    //                execvp(cmddd[0], cmddd);
    //            }
    //        }
    //    }
    //}

    //for (int i = 0; i < 4; i++) {
    //    close(pipefd[i]);
    //}

    //for (int i = 0; i < 2; i++) {
    //    wait(&status);
    //}
    int status;

#define NUM_PIPES 3

#define N (2*NUM_PIPES)

    if (fork() == 0) {
        int pipefd[N];

        for (int i = 0; i < N/2; i++) {
            pipe(pipefd+i*2);
        }

        for (int i = 0; i < N; i += 2) {
            if (fork() == 0) {
                if (i == 0) {
                    dup2(pipefd[i+1], STDOUT_FILENO);

                    for (int i = 0; i < N; i++) {
                        close(pipefd[i]);
                    }

                    execvp(cmd[0], cmd);
                } else if (i == N-2) {
                    //close(pipefd[i-1]);
                    dup2(pipefd[i-2], STDIN_FILENO);

                    for (int i = 0; i < N; i++) {
                        close(pipefd[i]);
                    }

                    execvp(cmdd[0], cmdd);
                } else {
                    //close(pipefd[i-1]);
                    //close(pipefd[i]);
                    dup2(pipefd[i-2], STDIN_FILENO);
                    dup2(pipefd[i+1], STDOUT_FILENO);

                    //close(pipefd[i-2]);
                    //close(pipefd[i+1]);
                    //dup2(pipefd[i], STDIN_FILENO);
                    //dup2(pipefd[i-1], STDOUT_FILENO);

                    for (int i = 0; i < N; i++) {
                        close(pipefd[i]);
                    }

                    //close(pipefd[i-2]);
                    //close(pipefd[i+1]);

                    execvp(cmddd[0], cmddd);
                }
            }
        }

        for (int i = 0; i < N; i++) {
            close(pipefd[i]);
        }

        for (int i = 0; i < N; i++) {
            wait(&status);
        }

        //exit(EXIT_SUCCESS);
    }

    //for (int i = 0; i < 4; i++) {
    //    close(pipefd[i]);
    //}

    for (int i = 0; i < 2; i++) {
        wait(&status);
    }

    /*
    if (fork() == 0) {

    pipe(pipefd);

    if (fork() == 0) {
    close(pipefd[R]);
    dup2(pipefd[W], STDOUT_FILENO);
    execvp(cmd[0], cmd);
    } else {
    close(pipefd[W]);
    dup2(pipefd[R], STDIN_FILENO);
    execvp(cmdd[0], cmdd);
    }

    //close(pipefd[R]);
    };

    for (int i = 0; i < 2; i++) {
    wait(&status);
    }
    */

    //char buf;
    //
    //if (argc != 2) {
    //    fprintf(stderr, "Usage: %s <string>\n", argv[0]);
    //    exit(EXIT_FAILURE);
    //}

    //if (pipe(pipefd) == -1) {
    //    perror("pipe");
    //    exit(EXIT_FAILURE);
    //}

    //cpid = fork();
    //if (cpid == -1) {
    //    perror("fork");
    //    exit(EXIT_FAILURE);
    //}

    //if (cpid == 0) {    /* Child reads from pipe */
    //    close(pipefd[1]);          /* Close unused write end */

    //    while (read(pipefd[0], &buf, 1) > 0)
    //        write(STDOUT_FILENO, &buf, 1);

    //    write(STDOUT_FILENO, "\n", 1);
    //    close(pipefd[0]);
    //    _exit(EXIT_SUCCESS);

    //} else {            /* Parent writes argv[1] to pipe */
    //    close(pipefd[0]);          /* Close unused read end */
    //    write(pipefd[1], argv[1], strlen(argv[1]));
    //    close(pipefd[1]);          /* Reader will see EOF */
    //    wait(NULL);                /* Wait for child */
    //    exit(EXIT_SUCCESS);
    //}
}