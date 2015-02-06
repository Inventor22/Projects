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
    char* cmd[] = {"ls", NULL};
    char* cmdd[] = {"grep", "a", NULL};
    char* cmddd[] = {"grep", "R", NULL};

    int status;

#define NUM_PIPES 2

#define N (2*NUM_PIPES)

    if (fork() == 0)
    {
        int* pipefd = (int*)malloc(sizeof(int) * N);

        for (int i = 0; i < NUM_PIPES; i++)
        {
            pipe(pipefd+i*2);
        }

        for (int i = 0; i < N+1; i += 2)
        {
            if (fork() == 0)
            {
                if (i == 0)
                {
                    dup2(pipefd[i+1], STDOUT_FILENO);

                    for (int i = 0; i < N; i++)
                    {
                        close(pipefd[i]);
                    }

                    execvp(cmd[0], cmd);
                }
                else if (i == N)
                {
                    dup2(pipefd[i-2], STDIN_FILENO);

                    for (int i = 0; i < N; i++)
                    {
                        close(pipefd[i]);
                    }

                    execvp(cmdd[0], cmdd);
                }
                else
                {
                    dup2(pipefd[i-2], STDIN_FILENO);
                    dup2(pipefd[i+1], STDOUT_FILENO);

                    for (int i = 0; i < N; i++)
                    {
                        close(pipefd[i]);
                    }

                    execvp(cmddd[0], cmddd);
                }
            }
        }

        for (int i = 0; i < N; i++)
        {
            close(pipefd[i]);
        }

        for (int i = 0; i < N; i++)
        {
            wait(&status);
        }
    }

    wait(NULL);
    //for (int i = 0; i < N; i++)
    //{
    //    wait(&status);
    //}
}