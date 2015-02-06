#include <iostream>

using namespace std;

/*
This will effectively take a line like "ls -la" and turn it into argv** = {"ls", "-la"}
*/
void  parse(char *line, char **argv)
{
    while (*line != '\0')
    {      /* if not the end of line ....... */
        while (*line == ' ' || *line == '\t' || *line == '\n' || *line == '\r') // added '\r' option.  Another way to do this: strchr(" \t\r\n", *line) != NULL
            *line++ = '\0';      /* replace white spaces with 0    */
        *argv++ = line;          /* save the argument position     */
        while (*line != '\0' && *line != ' ' &&
               *line != '\t' && *line != '\n')
               line++;              /* skip the argument until ...    */
    }
    *argv = '\0';                /* mark the end of argument list  */
}

int main()
{
    char line[1000];
    gets(line);

    char* str = (char*)malloc(sizeof(char) * strlen("ls -la"));
    strcpy(str, "ls -la");
    printf("%s\n", str);

    char* ss[15];
    parse(line, ss);

    printf("%s\n",*ss);

    int i = 0;
    while (ss[i] != NULL)
    {
        printf("%d: %s\n", i, ss[i]);
        i++;
    }
    getchar();
}