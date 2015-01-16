/**
* File:      calculator.c
* Name:      Dustin Dobransky
* ID:        ddobran
* Student #: 250575030
* Class:     CS3357 - Computer Networks=
*
* Created on October 9, 2014
*
* Description:
*      See calculator.h
*/


#include "calculator.h"

#define DIVIDED_BY_0 true

/// <summary>
/// Removes the spaces.
/// </summary>
/// <param name="exp">The expression.</param>
/// <returns>The length of the returned string</returns>
int removeSpaces(char* exp) {
    char cpy[80];
    int index = 0;
    int count = 0;
    while (exp[index] != '\0') {
        if (!isspace(exp[index])) {
            cpy[count++] = exp[index];
        }
        ++index;
    }
    cpy[count] = '\0';
    memcpy(exp, cpy, count + 1);
    return strlen(cpy);
}

/// <summary>
/// Finds the unary operators and marks them with a '_'
/// to help the parser identify the unary operators
/// from the '-' (minus) operators.
/// Took me way to long to figure out how to deal with
/// unary operators...
/// </summary>
/// <param name="e">The expression</param>
void findUnaryOperators(char* e) {
    int i = 0;
    bool prevCharWasop = true;
    while (e[i] != '\n') {
        if (e[i] == '-' || e[i] == '(' || e[i] == '+' ||
            e[i] == '*' || e[i] == '/') {
            if (prevCharWasop &&
                (e[i] == '-' ||
                 e[i] == '+' ||
                 e[i] == '*' ||
                 e[i] == '/')) {
                e[i] = '_';
            }
            prevCharWasop = true;
        } else {
            prevCharWasop = false;
        }
        i++;
    }
}

/// <summary>
/// Gets the precendence of an operator
/// </summary>
/// <param name="c">The operator.</param>
/// <returns>The precedence of the operator</returns>
int getPrecendence(char c) {
    if (c == '_') // unary '-'
        return 2;
    if (c == '*' || c == '/')
        return 1;
    if (c == '+' || c == '-')
        return 0;
    return 0;
}

/// <summary>
/// Evaluate_next_operator for the next operator.
/// </summary>
/// <param name="operands">The operands.</param>
/// <param name="operators">The operators.</param>
bool evaluate_next_operator(struct Stack* operands, struct Stack* operators) {
    char op = Stack_Top(operators);
    Stack_Pop(operators);

    if (op == '_') { // deal with unary '-' differently
        int val = -Stack_Top(operands);
        Stack_Pop(operands);
        Stack_Push(operands, val);
        return;
    }

    int opr2 = Stack_Top(operands);
    Stack_Pop(operands);
    int opr1 = Stack_Top(operands);
    Stack_Pop(operands);

    switch (op) {
        case '*':
            Stack_Push(operands, opr1 * opr2);
            break;
        case '/':
            if (opr2 == 0) {
                syslog(LOG_ERR, "Cannot divide by 0");
                return DIVIDED_BY_0;
            }
            Stack_Push(operands, opr1 / opr2);
            break;
        case '+':
            Stack_Push(operands, opr1 + opr2);
            break;
        case '-':
            Stack_Push(operands, opr1 - opr2);
            break;
    }
    return false;
}

/// <summary>
/// Evaluate_exps evaluates the specified expression.
/// </summary>
/// <param name="exp">The expression.</param>
/// <returns>The evaluated result</returns>
struct Result evaluate_expr(char* exp) {
    static const char* ods = "0123456789(";
    static const char* ops = "+-*/_"; // '_' is unary '-'
    struct Stack operands;
    struct Stack operators;
    Stack_Init(&operators);
    Stack_Init(&operands);

    removeSpaces(exp);
    findUnaryOperators(exp);

    //syslog(LOG_INFO, "parsing: %s", exp);

    int i;
    i = 0;
    while (exp[i] != '\0') {
        char c = exp[i++];
        if (strchr(ods, c) != NULL) { // if char == operand
            if (c == '(') {
                Stack_Push(&operators, c);
            } else {
                int iStart = i - 1;
                int iEnd = iStart;
                do {
                    c = exp[i++];
                } while (c != '\n' && isdigit(c));
                iEnd = --i;
                char num[MAX_DIGITS] = "";
                strncat(num, exp + iStart, iEnd - iStart);
                Stack_Push(&operands, atoi(num));
            }
        } else if (strchr(ops, c) != NULL) { // if char == operator
            int currentPres = getPrecendence(c);
            while (Stack_Size(&operators) > 0 &&
                   Stack_Size(&operands)  > 0 &&
                   currentPres < getPrecendence((char)Stack_Top(&operators))) {
                if (evaluate_next_operator(&operands, &operators) == DIVIDED_BY_0) {
                    return inf;
                }
            }
            Stack_Push(&operators, c);
        } else if (c == ')') {
            while (Stack_Top(&operators) != '(') {
                if (evaluate_next_operator(&operands, &operators) == DIVIDED_BY_0) {
                    return inf;
                }
            }
            Stack_Pop(&operators);
        }
    }

    while (Stack_Size(&operators) > 0) {
        if (evaluate_next_operator(&operands, &operators) == DIVIDED_BY_0) {
            return inf;
        }
    }

    int resultInt = Stack_Top(&operands);
    syslog(LOG_INFO, "Result: %d", resultInt);
    struct Result result = {resultInt, false};
    return result;
}



