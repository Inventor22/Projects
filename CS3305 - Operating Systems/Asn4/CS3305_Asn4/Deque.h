/*
Name: Dustin Dobransky
Id:   250575030
Date: 11/14/15

Description:
    Header file for a Deque implemnentation.
    A Deque is a queue which allows pushing/popping from both ends.
*/

#ifndef DEQUE_H
#define DEQUE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct Node
{
    struct Node* next;
    struct Node* prev;
    int data;
} Node;

typedef struct Deque
{
    struct Node* head;
    struct Node* tail;
    int size;
} Deque;

Node*  makeNode(int val);
Deque* getDeque();
bool   pushTop(Deque* d, int item);
int    popTop(Deque* d);
bool   pushBottom(Deque* d, int item);
int    popBottom(Deque* d);
void   removeItem(Deque* d, int item);
void   freeDeque(Deque* d);

#endif