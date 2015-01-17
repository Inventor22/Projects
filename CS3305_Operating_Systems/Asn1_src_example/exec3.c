#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
   Illustrates the use of execlp;  "ls" is to be executed"

*/
 
int main()
{


  int i=5;
  int status;

  status = execlp("ls", "ls" "-lt", "B.c", NULL);
  if (status < 0){
    perror("exec");
    printf("main: errno is %d\n",errno);
    exit(status);
  }

  printf("%d\n",i);
}
