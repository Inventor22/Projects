#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*
   This program opens file twice; does two reads and prints the result of 
   second read
*/

int main()
{

  FILE *fd1, *fd2;
  char c;

  fd1 = fopen("foobar.txt", "r");
  if (fd1 == NULL){
   perror("file open error");
   printf("errno is %d\n", errno);
   exit(-1);
  }

  fd2 = fopen("foobar.txt", "r");
  if (fd2 < 0){
   perror("file open error");
   printf("errno is %d\n", errno);
   exit(-1);
  }


  fscanf(fd1, "%c", &c);
  fscanf(fd2, "%c", &c);
  printf("c = %c\n", c);
}
