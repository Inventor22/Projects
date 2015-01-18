//#include <stdio.h>
//#include <sys/types.h>
//#include <sys/wait.h>
//#include <unistd.h>
//#include <string.h>
//
//#define MAX 256
//#define CMD_MAX 10
//
//int make_tokenlist(char *buf, char *tokens[]) {
//    char input_line[MAX];
//    char *line;
//    int i, n;
//
//    i = 0;
//
//    line = buf;
//    tokens[i] = strtok(line, " ");
//    do {
//        i++;
//        line = NULL;
//        tokens[i] = strtok(line, " ");
//    } while (tokens[i] != NULL);
//
//    return i;
//}
//
//
//int main(void) {
//
//    int quit = 0;
//
//    while (!quit) {
//        pid_t child0;
//
//        if ((child0 = fork()) == 0) { // create child process
//            // parse input command
//
//            if (strcmp(tokens[0], "exit") == 0) {
//
//            }
//
//
//        } else { // parent process
//            wait(NULL);
//        }
//
//    }
//
//}
//
