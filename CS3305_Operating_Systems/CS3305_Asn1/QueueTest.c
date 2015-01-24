#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "HistoryQueue.h"

int main() {
    HistoryQueue* q;
    q = initHistoryQueue(10);
    enqueue(q, "Hello");
    enqueue(q, "World");
    printHistory(q, 10);
    printf("commands: %s\n", dequeue(q));
    printf("commands: %s\n", dequeue(q));

    freeHistoryQueueData(q);

    char s[10];
    for (int i = 0; i <= 20; i++) {
        _itoa_s(i,s,10,10);
        enqueue(q, s);
    }
    printHistory(q);

    getchar();
}