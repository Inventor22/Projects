#include <sys/types.h>  // (pid_t typedef)
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(void){
  int fd[2];
  int n;
  char c;
  pid_t pid;
  char line[80];
  int status;
 
  if (pipe(fd) < 0){
    perror("pipe error");
    exit(-1);
  } 

  pid = fork();
  if (pid < 0) {
    perror("fork error");
    exit(-1);
  }

  if(pid > 0)
  {
    close(fd[0]);
    write(fd[1], "hello world\n",12);
    wait(NULL);
  } else {
    close(fd[1]);
    n = read(fd[0],line,80);
    write(1,line,n);
  }

  return 0;

}
