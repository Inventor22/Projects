#ifndef HISTORYQUEUE_H
#define HISTORYQUEUE_H

#include <string.h>
#include <stdio.h>      /* printf, scanf, NULL */
#include <stdlib.h>     /* malloc, free, rand */

/* refresher on C syntax and queues: http://www.cs.ucf.edu/courses/cop3502/nihan/spr03/queue.pdf */

#define MAX_CHARS 256
#define CMD_MAX 16
// 10
#define MAX_HISTORY 100

typedef struct StringQueue {
    int size;
    int count;
    int start, end;
    char (*history)[MAX_CHARS]; // pointer to MAX_CHARS-element array of char
} StringQueue;

StringQueue*  initStringQueue(int size);
char*         topStringQueue(StringQueue* q);
char*         dequeueString(StringQueue* q);
void          enqueueString(StringQueue* q, char* msg);
void          freeStringQueueData(StringQueue* q);
int           stringQueueCount(StringQueue* q);
void          printStringQueue(StringQueue* q, int n);

#endif