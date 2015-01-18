#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include "HistoryQueue.h"

int main() {
    HistoryQueue* q = initHistoryQueue(10);
    enqueue(q, "Hello");
    enqueue(q, "World");
    printHistory(q);
    getchar();
}