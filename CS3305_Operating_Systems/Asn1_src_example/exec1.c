#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
  Simple program that uses execl

*/

int main()
{

  int i = 5;
  int status;

  printf("%d\n",i);

  status = execl("B","B",NULL);

  if (status < 0){
    perror("exec");
    printf("main: errno is %d\n",errno);
    exit(status);
  }
  printf("%d\n",i);
}
