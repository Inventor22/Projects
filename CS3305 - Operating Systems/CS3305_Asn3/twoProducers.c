/*
Author: Dustin Dobransky
id:     250575030
date:   23/03/15
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

/*
In this part of the assignment you will develop a program for a variation of the
“Consumer-Producer Problem”. Assume one consumer and two producers. Each
producer has a buffer that it shares with a consumer. Buffers may be of different sizes.
A consumer takes one item from either buffer. Only if both buffers are empty is the
consumer blocked. It should be unblocked when any of the buffers receive an item.
You should use semaphores as the synchronization mechanism and there should be no
busy waiting.
The program should be called twoProducers.c. This program will take four arguments.
The first two arguments represent the buffer sizes of the buffers associated with the
producers while the last two arguments represent the number of items produced by
each of the producers.
When a producer puts something in the buffer you should have a print statement
indicating this e.g., “producer 1 has put an item in its buffer”, “producer 2 has put an
item in its buffer”. When a consumer has consumed an item then you should have a
print statement indicating this e.g., “Consumer has consumed an item from producer
1” , “Consumer has consumed an item from producer 2”
*/

sem_t queue; // start at 0 --> no items in the queue.  Cannot 'wait' on two buffers at once (full),
             // so both producers have to report to a queueing semaphore to keep track of total production count.
sem_t empty[2]; // start at N, parameter given on command line
sem_t full[2];  // start at 0 (producers have not produced anything yet)

int totalProducts;

typedef struct
{
    int numItemsToProduce;
    int size;
    int count, in, out;
    int* buf;
    int id;
} buffer;

#define PROD_0 0
#define PROD_1 1

buffer b[2];

/* Producer calls this to generate
   something to put in buffer
   • Needs to determine next available slot
   • Needs to write */
int produceItem()
{
    return rand()%100;
}

void insertItem(buffer* b, int item)
{
    b->buf[(b->in++ + 1) % b->size] = item;
    b->in %= b->size;
    b->count++;
    printf("Producer %d has put an item in its buffer\n", b->id);
}

// Consumer calls to take item from buffer
int removeItem(buffer* b)
{
    int item = b->buf[b->out++];
    b->out %= b->size;
    b->count--;
    return item;
}

// Consumer calls this to do something with item
void consumeItem(int producer)
{
    printf("Consumer has consumed an item from producer %d\n", producer);
}

// produce items, report quantities to overall queue and personal buffer
void* producer(void* args)
{
    buffer* b = (buffer*)args;
    int item;
    while (b->numItemsToProduce-- > 0) {
        item = produceItem();

        sem_wait(&empty[b->id]); // wait until consumer consumes

        insertItem(b, item);

        sem_post(&full[b->id]); // report production run to own buffer
        sem_post(&queue); // report production run to overall queue
        int a;
        sem_getvalue(&queue, &a);
        printf("%d prod queue: %d\n", b->id, a);
    }
    printf("finished producing %d\n", b->id);
    int v[5];
    sem_getvalue(&empty[0], &v[0]);
    sem_getvalue(&empty[1], &v[1]);
    sem_getvalue(&full[0], &v[2]);
    sem_getvalue(&full[1], &v[3]);
    sem_getvalue(&queue, &v[4]);
    printf("empty0: %d, empty1: %d, full0: %d, full1: %d, queue: %d\n", v[0], v[1], v[2], v[3], v[4]);
    //pthread_exit(NULL);
}

void* consumer(void* args)
{
    buffer* b = (buffer*)args;
    int item;
    int i = 0;
    int consumedProducts = 0;
    while (consumedProducts < totalProducts) {
        int a;
        sem_getvalue(&queue, &a);
        //printf("%d\n", a);
        sem_wait(&queue); // halt until there is stuff in either buffer

        int val[2];
        sem_getvalue(&full[0], &val[0]);
        sem_getvalue(&full[1], &val[1]);

        if (val[0] > 0 || val[1] > 0) {
            i = rand()&1; // if both buffers have stuff, select random one
        } else if (val[0] > 0) {
            i = 0;
        } else if (val[1] > 0) {
            i = 1;
        }

        sem_wait(&full[i]);

        item = removeItem((void*)&b[i]);

        sem_post(&empty[i]);

        consumeItem(i);
        consumedProducts++;
    }
    printf("done");
    pthread_exit(NULL);
}

void* f(void* a)
{
    for (int i = 0; i < 10; i++) {
        sleep(1000);
        int v[5];
        sem_getvalue(&empty[0], &v[0]);
        sem_getvalue(&empty[1], &v[1]);
        sem_getvalue(&full[0], &v[2]);
        sem_getvalue(&full[1], &v[3]);
        sem_getvalue(&queue, &v[4]);
        printf("empty0: %d, empty1: %d, full0: %d, full1: %d, queue: %d\n", v[0], v[1], v[2], v[3], v[4]);
        /*    int numItemsToProduce;
        int size;
        int count, in, out;
        int* buf;
        int id;*/
        printf("id: %d, size: %d, count: %d, in: %d, out: %d\n", b[0].id, b[0].size, b[0].count, b[0].in, b[0].out);
        printf("id: %d, size: %d, count: %d, in: %d, out: %d\n", b[1].id, b[1].size, b[1].count, b[1].in, b[1].out);
    }
}

int main(int argc, char** argv)
{
    if (argc != 5) {
        printf("Must have 4 params");
        exit(EXIT_FAILURE);
    }

    totalProducts = 0;
    for (int i = 0; i < 2; i++) {
        b[i].size = atoi(argv[i*2+1]);
        b[i].numItemsToProduce = atoi(argv[i*2+2]);
        totalProducts += b[i].numItemsToProduce;
        b[i].buf = (int*)malloc(sizeof(int) * b[i].size);
        b[i].in = 0;
        b[i].out = 0;
        b[i].count = 0;
        b[i].id = i;
    }

    if (sem_init(&queue, 0, 0)) {
        printf(("Error creating semaphore.\n"));
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; i++) {
        if (sem_init(&empty[i], 0, b[i].size)) {
            printf(("Error creating semaphore.\n"));
            exit(EXIT_FAILURE);
        }
        if (sem_init(&full[i], 0, 0)) {
            printf(("Error creating semaphore.\n"));
            exit(EXIT_FAILURE);
        }
    }

    pthread_t thread[3];

    if (pthread_create(&thread[0], NULL, producer, (void*)&b[PROD_0])) {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread[1], NULL, producer, (void*)&b[PROD_1])) {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }
    if (pthread_create(&thread[2], NULL, consumer, (void*)&b)) {
        fprintf(stderr, "Error while creating thread\n");
        exit(1);
    }

    pthread_t a;
    pthread_create(&a, NULL, f, (void*)NULL);

    for (int i = 0; i < 3; i++)
        pthread_join(thread[i], NULL);

    free(b[0].buf);
    free(b[1].buf);
    return EXIT_SUCCESS;
}