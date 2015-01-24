#ifndef STACK_H
#define STACK_H

// retrieved from: http://groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/stack1.html
// Added Stack_Size function

#include "HistoryQueue.h"

#define STACK_MAX 10

/// <summary>
/// The Stack data structure
/// </summary>
struct Stack {
    /*
    Stack of maximum 10 elements (STACK_MAX),
    holding an array of size 10 (CMD_MAX) strings,
    where each string is of maximum length 256 (MAX_CHARS) chars long.
    */
    char    data[STACK_MAX][CMD_MAX][MAX_CHARS]; // string of strings
    int     size;
};
typedef struct Stack Stack;

void Stack_Init(Stack* s);

char** Stack_Top(Stack* s);

void Stack_Push(Stack* s, char** tokens);

void Stack_Pop(Stack* s);

int Stack_Size(Stack* s);

#endif
