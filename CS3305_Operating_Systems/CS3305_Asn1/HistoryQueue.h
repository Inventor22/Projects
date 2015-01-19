#ifndef HISTORYQUEUE_H
#define HISTORYQUEUE_H

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

/* refresher on C syntax and queues: http://www.cs.ucf.edu/courses/cop3502/nihan/spr03/queue.pdf */

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
void          freeHistoryQueueData(HistoryQueue* q);
void          printHistory(HistoryQueue* q, int n);

#endif