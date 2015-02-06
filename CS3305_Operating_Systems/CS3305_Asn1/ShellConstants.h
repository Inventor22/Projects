#ifndef SHELLCONSTANTS_H
#define SHELLCONSTANTS_H

// example: command -a -b -c -d .... -l (max 12 commands)
#define MAX_ARGS_PER_CMD 12

// total number of commands.
// Example:  "ls -a | grep a | greb b" == 3 commands
#define CMD_MAX          32

#define MAX_HISTORY      100

#endif