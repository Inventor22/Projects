#include "Stack.h"
#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>

/// <summary>
/// Initializes the stack
/// </summary>
/// <param name="s">The stack pointer.</param>
void Stack_Init(Stack* s) {
    s->size = 0;
}

/// <summary>
/// retrieved the top stack element
/// </summary>
/// <param name="s">The stack pointer.</param>
/// <returns>The top stack element</returns>
char** Stack_Top(Stack* s) {
    if (s->size == 0) {
        syslog(LOG_ERR, "Stack empty, cannot get Top!");
        exit(EXIT_FAILURE);
    }

    return s->data[s->size - 1];
}

/// <summary>
/// Pushes an int onto the stack
/// </summary>
/// <param name="s">The stack pointer.</param>
/// <param name="d">The int to be pushed.</param>
void Stack_Push(Stack* s, char* tokens[]) {
    if (s->size < STACK_MAX)
        strcpy(s->data[s->size++], tokens);
    else {
        fprintf(stderr, "Error: stack full\n");
        syslog(LOG_ERR, "Cannot push onto a full stack!\n");
    }
}

/// <summary>
/// Pops an item from the top of the stack
/// </summary>
/// <param name="s">The stack pointer.</param>
void Stack_Pop(Stack* s) {
    if (s->size == 0) {
        fprintf(stderr, "Error: stack empty\n");
        syslog(LOG_ERR, "Cannot pop from an empty stack!\n");
    }
    else
        s->size--;
}

/// <summary>
/// Size of the stack.
/// </summary>
/// <param name="s">The stack pointer.</param>
int Stack_Size(Stack* s) {
    return s->size;
}
