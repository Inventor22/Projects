#include "StringQueue.h"

StringQueue* initStringQueue(int size) {
    StringQueue* h = (StringQueue*)malloc(sizeof(StringQueue));
    h->size = size;
    h->start = 0;
    h->end = -1;
    h->count = 0;
    // allocate memory for (size * sizeof(char) * MAX_CHARS) bytes.
    // pointer offset will access different elements in the array.
    // to deallocate, simply call free(h->history)
    h->history = malloc(sizeof(h->history[0]) * size);
    return h;
}

void freeStringQueueData(StringQueue* q) {
    free(q->history);
    q->count = 0;
    q->start = 0;
    q->end = -1;
}

char* topStringQueue(StringQueue* q) {
    return q->history[q->start];
}

char* dequeueString(StringQueue* q) {
    if (q->count > 0) {
        char* msg = q->history[q->start];
        q->start = (q->start + 1) % q->size;
        q->count--;
        return msg;
    } else {
        return NULL;
    }
}

void enqueueString(StringQueue* q, char* msg) {
    q->end = (q->end + 1) % q->size;
    if (q->end == q->start && q->count >= q->size) {
        q->start = (q->end+1)%q->size; // re-write over last element if queue is full
    }
    if (q->count < 10) {
        q->count++;
    }
    //strcpy_s(q->history[q->end], MAX_CHARS, msg); // copy msg to appropriate location in msg array
    strcpy(q->history[q->end], msg); // copy msg to appropriate location in msg array
}

int stringQueueCount(StringQueue* q) {
    return q->count;
}

void printStringQueue(StringQueue* q, int n) {
    int j = 1;
    if (n < q->count) {
        for (int i = 0; i < n; i++) {
            printf("Command %d: %s", j++, q->history[(q->end-n+i)%q->size]);
        }
    } else {
        for (int i = 0; i < q->count; i++) {
            printf("Command %d: %s", j++, q->history[(q->start+i)%q->size]);
        }
    }
}