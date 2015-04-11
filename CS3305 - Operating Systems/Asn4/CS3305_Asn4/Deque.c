/*
Name: Dustin Dobransky
Id:   250575030
Date: 11/04/15

Description:
    Impementation for a Deque implemnentation.
    A Deque is a queue which allows pushing/popping from both ends.
*/

#include "Deque.h"

Node* makeNode(int val)
{
    Node* n = (Node*)malloc(sizeof(Node)); // allocate memory for a Node
    n->data = val; // initialize its values
    n->next = n->prev = NULL;
    return n;
}

Deque* getDeque()
{
    Deque* d = (Deque*)malloc(sizeof(Deque)); // allocate memory for a Deque
    d->size = 0;
    d->head = d->tail = NULL; // initialize its values
    return d;
}

/*
Push a new node onto the top of the Deque
*/
bool pushTop(Deque* d, int item)
{
    if (d == NULL) return false;

    Node* n = makeNode(item);

    if (d->head == NULL) {     // if empty
        d->head = d->tail = n; // make head and tail equal
    } else {                   // otherwise append new node
        d->head->prev = n;
        n->next = d->head;
    }
    d->head = n;
    d->size++;
    return true;
}

/*
Pop a node off the top of the Deque
*/
int popTop(Deque* d)
{
    if (d == NULL) {
        exit(EXIT_FAILURE);
    }

    Node* n = d->head;
    int val = n->data;

    if (n->next == NULL) { // if only one node
        d->head = d->tail = NULL;
    } else {               // if multiple nodes
        d->head = d->head->next;
        d->head->prev = NULL;
    }

    d->size--;
    free(n);

    return val;
}

/*
Push a node onto the bottom of the Deque
*/
bool pushBottom(Deque* d, int item)
{
    if (d == NULL) return false;

    Node* n = makeNode(item);

    if (d->tail == NULL) {     // empty Deque
        d->head = d->tail = n; // head and tail are equal
    } else {                   // otherwise append node
        d->tail->next = n;
        n->prev = d->tail;
    }
    d->tail = n;
    d->size++;
    return true;
}

/*
Pop a node off the bottom of the Deque
*/
int popBottom(Deque* d)
{
    if (d == NULL) {
        exit(EXIT_FAILURE);
    }

    Node* n = d->tail;
    int val = n->data;

    if (n->prev == NULL) {        // empty Deque
        d->head = d->tail = NULL; // head and tail are equal to NULL
    } else {
        d->tail = d->tail->prev;  // otherwise remove
        d->tail->next = NULL;
    }

    d->size--;
    free(n);

    return val;
}

/*
Remove an item from the Deque
*/
bool removeItem(Deque* d, int item)
{
    Node* h = d->head; // get pointer to head and tail of Deque
    Node* t = d->tail; // iterate front to back and back to front simultaneously
    while (true) {
        if (h->data == item) { // iterate forwards
            if (h->prev == NULL) {
                popTop(d);
            } else {
                Node* n = h;
                n->prev->next = h->next;
                n->next->prev = h->prev;
            }
            return true;
        } else if (t->data == item) { // iterate backwards
            if (t->next == NULL) {
                popBottom(d);
            } else {
                Node* n = t;
                n->prev->next = t->next;
                n->next->prev = t->prev;
            }
            return true;
        }

        h = h->next;

        // if pointing to same value, i.e. meet in the middle of the Deque, return
        if (h == t) return false;

        t = t->prev;

        // if end of deque has been reached (shouldn't occur because of if(h == t)
        if (h == NULL || t == NULL) return false;
    }

    return false;
}

/*
Free all allocated memory for the Deque
*/
void freeDeque(Deque* d)
{
    if (d == NULL) return;
    if (d->head == NULL) return;
    while (d->head->next != NULL) {
        Node* n = d->head;
        d->head = d->head->next;
        free(n);
    }
    free(d);
}