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
    q->count = 0;
    q->start = 0;
    q->end = -1;
}

char* dequeue(HistoryQueue* q) {
    if (q->count > 0) {
        char* msg = q->history[q->start];
        q->start = (q->start + 1) % q->size;
        q->count--;
        return msg;
    } else {
        return NULL;
    }
}

void enqueue(HistoryQueue* q, char* msg) {
    q->end = (q->end + 1) % q->size;
    if (q->end == q->start && q->count >= q->size) {
        q->start = (q->end+1)%q->size; // re-write over last element if queue is full
    }
    if (q->count < 10) {
        q->count++;
    }
    strcpy_s(q->history[q->end], MAX_CHARS, msg); // copy msg to appropriate location in msg array
}

void printHistory(HistoryQueue* q, int n) {
    int j = 1;
    if (n < q->count) {
        for (int i = 0; i < n; i++) {
            printf("Command %d: %s\n", j++, q->history[(q->end-n+i)%q->size]);
        }
    } else {
        for (int i = 0; i < q->count; i++) {
            printf("Command %d: %s\n", j++, q->history[(q->start+i)%q->size]);
        }
    }
}