
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX 256
#define CMD_MAX 10

int make_tokenlist(char *buf, char *tokens[])
{

 char input_line[MAX];
 char *line;
 int i,n;

 i = 0;

 line = buf;
 tokens[i] = strtok(line, " ");
 do  {
    i++;
    line = NULL;
    tokens[i] = strtok(line, " ");
 } while(tokens[i] != NULL);

 return i;
}


/*
  The purpose of the main program is to illustrate the usage of
  the make_tokenlist function

*/

void main(void) 
{

   char input_line[MAX], *tokens[CMD_MAX];
   int i, n; 
  
   printf("hanan> ");
   if (fgets(input_line,MAX,stdin) != NULL) 
     n= make_tokenlist(input_line, tokens);
   else 
     printf("huh?\n");

   for (i = 0; i < n; i++)
     printf("extracted token is %s\n", tokens[i]);

  
}