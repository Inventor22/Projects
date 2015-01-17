#include <sys/types.h>  
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/*
  Illustrate the use of fork, exec and pipe2
*/

int main(void){
  int fd[2];
  int n;
  char c;
  pid_t pid;
  char line[80];
  int status;
 
  if (pipe(fd) < 0){
    perror("fatal error");
    exit(1);
  }
 
  pid = fork();
  if (pid < 0) {
    perror("fork error");
    exit(-1);
  }

  if(pid > 0) // parent process
  {
    close(fd[0]); // close READ
    if (dup2(fd[1], STDOUT_FILENO) < 0){ // change write to the piped process
      perror("can't dup");
      exit(1);
    }
    status = execlp("ps","ps","-le", NULL); // report process status
    if (status < 0){
       perror("parent: exec problem");
       exit(1);
    }
    wait(NULL); // wait for child process to continue
  } else { // child process
    close(fd[1]);
    if (dup2(fd[0], STDIN_FILENO) < 0){ // copy file descriptor table, making standard input read from the output of parent
      perror("can't dup");
      exit(1);
    }
    status = execlp("sort","sort", NULL); // sort data from 
    if (status < 0){
       perror("parent: exec problem");
       exit(1);

  }
 }
  return 0;

}
