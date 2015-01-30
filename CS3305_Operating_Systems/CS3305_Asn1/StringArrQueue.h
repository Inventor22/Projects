#ifndef STRINGARRQUEUE_H
#define STRINGARRQUEUE_H

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

#include "StringQueue.h"

#define MAX_TOKENS 10

/* refresher on C syntax and queues: http://www.cs.ucf.edu/courses/cop3502/nihan/spr03/queue.pdf */

typedef struct StringArrQueue {
    int size;
    int count;
    int start, end;
    /*
    create a pointer to an array of array of char.  When allocating memory for this construct,
    use history = malloc(size * MAX_TOKENS * MAX_CHARS);
    This will allocate size * 10 * 256 bytes of data.
    */
   //char(*history)[MAX_TOKENS][MAX_CHARS];
    char*** elem;
} StringArrQueue;

StringArrQueue* initStringArrQueue(int size);
char**        topStringArrQueue(StringArrQueue* q);
char**        dequeueStringArr(StringArrQueue* q);
char**        getStringArrElem(StringArrQueue* q, int index);
void          enqueueStringArr(StringArrQueue* q, char** tokens);
void          freeStringArrQueueData(StringArrQueue* q);
void          printStringArrQueue(StringArrQueue* q);

#endif