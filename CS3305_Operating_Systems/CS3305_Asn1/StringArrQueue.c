#include "StringArrQueue.h"

StringArrQueue* initStringArrQueue(int size) {
    StringArrQueue* h = (StringArrQueue*)malloc(sizeof(StringArrQueue));
    h->size = size;
    h->start = 0;
    h->end = -1;
    h->count = 0;
    h->elem = malloc(size * sizeof(h->elem[0]));
    return h;
}

void freeStringArrQueueData(StringArrQueue* q) {
    for (int i = 0; i < q->count; i++) {
        free(q->elem[i]);
    }
    free(q->elem);
    q->count = 0;
    q->start = 0;
    q->end = -1;
}

char** getStringArrElem(StringArrQueue* q, int index) {
    return q->elem[index];
}

char** topStringArrQueue(StringArrQueue* q) {
    return q->elem[q->start];
}

char** dequeueStringArr(StringArrQueue* q) {
    if (q->count > 0) {
        char** tokens = q->elem[q->start];
        q->start = (q->start + 1) % q->size;
        q->count--;
        return tokens;
    } else {
        return NULL;
    }
}

void enqueueStringArr(StringArrQueue* q, char** tokens) {
    q->end = (q->end + 1) % q->size;
    if (q->end == q->start && q->count >= q->size) {
        q->start = (q->end+1)%q->size; // re-write over last element if queue is full
    }
    if (q->count < 10) {
        q->count++;
    }
    //strcpy_s(q->elem[q->end], MAX_CHARS, msg); // copy msg to appropriate location in msg array
    q->elem[q->end] = tokens;
}

void printStringArrQueue(StringArrQueue* q)
{
    for (int i = 0; i < q->count; i++) {
        printf("Command %d:", i);
        int j = 0;
        while (q->elem[i][j] != NULL) {
            printf(" %s", q->elem[(q->start+i)%q->size][j]);
            j++;
        }
        printf("\n");
    }
}