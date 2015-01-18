#ifndef HISTORYQUEUE_H
#define HISTORYQUEUE_H

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

#define MAX_HISTORY 10
#define MAX_CHARS 256

typedef struct HistoryQueue {
    int size;
    int count;
    int start, end;
    char (*history)[MAX_CHARS];
} HistoryQueue;

HistoryQueue* initHistoryQueue(int size);
char*         dequeue(HistoryQueue* q);
void          enqueue(HistoryQueue* q, char* msg);
void          printHistory(HistoryQueue* q);

#endif