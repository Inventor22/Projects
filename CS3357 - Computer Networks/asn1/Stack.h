#ifndef STACK_H
#define STACK_H

// retrieved from: http://groups.csail.mit.edu/graphics/classes/6.837/F04/cpp_notes/stack1.html
// Added Stack_Size function

#define STACK_MAX 80

/// <summary>
/// The Stack data structure
/// </summary>
struct Stack {
    int     data[STACK_MAX];
    int     size;
};
typedef struct Stack Stack;

void Stack_Init(Stack* s);

int Stack_Top(Stack* s);

void Stack_Push(Stack* s, int d);

void Stack_Pop(Stack* s);

int Stack_Size(Stack* s);

#endif
