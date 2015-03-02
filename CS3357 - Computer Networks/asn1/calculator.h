/**
* File:      calculator.h
* Name:      Dustin Dobransky
* ID:        ddobran
* Student #: 250575030
* Class:     CS3357 - Computer Networks=
*
* Created on October 9, 2014
*
* Description:
*      This program evaluates simple arithmetic expressions containing only the
* following operators: '*', '/', '+', '-'.
* Support for unary '-' operator was also added.
*/

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include "Stack.h"
#include <syslog.h>

// assuming integers are 4 bytes wide
// 2^32 = 2Billion = 10 digits
#define MAX_DIGITS 10

struct Result {
    int result;
    bool isInfinite;
} Result;

static const struct Result inf = {0, true};

/// <summary>
/// Removes the spaces.
/// </summary>
/// <param name="exp">The expression.</param>
/// <returns>the length of the returned string</returns>
int removeSpaces(char* exp);

/// <summary>
/// Finds the unary operators and marks them with a '_'
/// to help the parser identify the unary operators
/// from the '-' (minus) operators.
/// Took me way to long to figure out how to deal with
/// unary operators...
/// </summary>
/// <param name="e">The expression</param>
void findUnaryOperators(char* e);

/// <summary>
/// Gets the precendence of an operator
/// </summary>
/// <param name="c">The operator.</param>
/// <returns>The precedence of the operator</returns>
int getPrecendence(char c);

/// <summary>
/// Evaluate_next_operator for the next operator.
/// </summary>
/// <param name="operands">The operands.</param>
/// <param name="operators">The operators.</param>
bool evaluate_next_operator(struct Stack* operands, struct Stack* operators);

/// <summary>
/// Evaluate_exps evaluates the specified expression.
/// </summary>
/// <param name="exp">The expression.</param>
/// <returns>The evaluated result</returns>
struct Result evaluate_expr(char* exp);

#endif
