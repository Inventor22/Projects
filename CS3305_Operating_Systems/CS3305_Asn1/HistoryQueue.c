#include "HistoryQueue.h"

HistoryQueue* initHistoryQueue(int size) {
    HistoryQueue* h = (HistoryQueue*)malloc(sizeof(HistoryQueue));
    h->size = size;
    h->start = 0;
    h->end = -1;
    h->count = 0;
    h->history = malloc(sizeof(h->history[0]) * size);
    return h;
}

void freeHistoryQueueData(HistoryQueue* q) {
    free(q->history);
}

char* dequeue(HistoryQueue* q) {
    if (q->count > 0) {
        q->start = (q->start - 1) % q->size;
        q->count--;
        return q->history[q->start];
    } else {
        return NULL;
    }
}

void enqueue(HistoryQueue* q, char* msg) {
    q->end = (q->end + 1) % q->size;
    if (q->end == q->start) {
        q->start = q->end; // re-write over last element if queue overflows
    }
    if (q->count < 10) {
        q->count++;
    }
    strcpy_s(q->history[q->end], MAX_CHARS, msg); // copy msg to appropriate location in msg array
}

void printHistory(HistoryQueue* q) {
    int j = 1;
    for (int i = q->start; i != q->end;) {
        printf("Command %d: %s\n", j++, q->history[i]);
        i = (i+1) % q->size;
    }
}