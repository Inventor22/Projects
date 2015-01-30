#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(void) {
    /*
    Example:  grep FindThisWord < file
    int fd_read = open("file", O_RDONLY);
    dup2(fd_read, STDIN_FILENO);
    execvp(tokens[0], tokens[0 to 1])

    Example 2:  grep FindThisWord < input > output
    int fd_read = open("input", O_RDONLY);
    int fd_write = open("output", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(fd_read, STDIN_FILENO);
    dup2(fd_write, STDOUT_FILENO);
    */
    char* args[] = {"grep", "dustin", (char*)NULL};

    //int input = open("file", O_RDONLY);
    //dup2(input, STDIN_FILENO);
    //close(input);
    //execvp(args[0], args);
    //getchar();


    int input = open("file", O_RDONLY);
    int output = open("output", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
    dup2(input, STDIN_FILENO);
    dup2(output, STDOUT_FILENO);
    close(input);
    close(output);
    execvp(args[0], args);
    getchar();

    /*   int input = open("file", O_RDONLY);
       int output = open("output2", O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
       dup2()
       */
}
