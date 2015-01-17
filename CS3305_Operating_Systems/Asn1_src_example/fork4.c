#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>


/*
  This program forks 3 times;  You can follow the print statements to see the
  tree created
*/

   int main()
   {
    pid_t childpid = 0;
    int i;
    pid_t parent;
    int n = 5;
    int count = 0;

    printf("first process has pid %d\n",getpid());
    i = fork();
    if (i>0) printf("child created here has pid %ld and parent has pid %ld\n",(long) i, (long) getpid());
    i= fork();
    if (i>0) printf("child created here has pid %ld and parent has pid %ld\n",(long) i, (long) getpid());
    i= fork();
    if (i>0) printf("child created here has pid %ld and parent has pid %ld\n",(long) i, (long) getpid());
}
