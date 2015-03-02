#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
  Use of fork and exec
*/

int main()
{

  pid_t pid;
  char *prog_argv[3];
  int status;

  prog_argv[0] = "ls";
  prog_argv[1] = "-lt";
  prog_argv[2] = NULL;

 
  pid = fork();
  if (pid < 0){
   perror("Fork()");
   printf("errno is %d\n", errno); 
   exit(pid);
  }

  if (pid > 0)
  {
     wait(NULL);
     printf("child completed \n");
  } else {
    if (pid == 0)
      execvp(prog_argv[0],prog_argv);
      if (status < 0){
        perror("exec");
        printf("main: errno is %d\n",errno);
        exit(status);
      }

 } 
   
}
