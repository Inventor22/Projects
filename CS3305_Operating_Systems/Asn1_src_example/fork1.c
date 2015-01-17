#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

/*  

   This program forks a process.  The child and parent processes print to
   to terminal to identify themselves

*/

int main()
{ 
  pid_t pid;          
  int i;
         

  pid = fork();
  if (pid < 0){
    perror("fork");
    printf("main function: errno number is %d\n",errno);
    exit(pid);
  }
  if (pid > 0){
   printf("I am parent\n"); 
   wait(NULL);
  }
  else { 
   printf("I am child\n"); 
  }
  return 0;
}

