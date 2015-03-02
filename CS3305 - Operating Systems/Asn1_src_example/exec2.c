#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

/*
   Illustrate execvp
*/

int main()
{

  int i = 5;
  char *prog_argv[2];
  int status;

  prog_argv[0] = "/home/user/Lectures/Exec/B";
  prog_argv[1] = NULL;
  printf("%d\n",i);

  status = execvp(prog_argv[0],prog_argv);
  if (status !=0) {
     perror("you goofed\n");
     printf("errno is %d\n",errno);
     exit(status);
  }
  printf("%d\n",i);
}
