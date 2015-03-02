/*
File:       StringArrQueue.h
Date:       01/02/15
Name:       Dustin Dobransky
Student ID: 250575030
Alias:      ddobran

Description:
    Too lazy to figure out how to make a generic queue/list class in C,
    so I wrote two implementations, and hardcoded the type.
*/

#ifndef STRINGARRQUEUE_H
#define STRINGARRQUEUE_H

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

#include "StringQueue.h"

typedef struct StringArrQueue
{
    int size;
    int count;
    int start, end;
    // pointer to pointer to pointer to char;
    // Equivalently, a pointer to an array of pointers,
    // which point to an array of char (strings)
    char*** elem;
} StringArrQueue;

StringArrQueue* initStringArrQueue(int size);
char**          topStringArrQueue(StringArrQueue* q);
char**          dequeueStringArr(StringArrQueue* q);
char**          getStringArrElem(StringArrQueue* q, int index);
void            enqueueStringArr(StringArrQueue* q, char** tokens);
void            freeStringArrQueueData(StringArrQueue* q);
void            printStringArrQueue(StringArrQueue* q);

#endif