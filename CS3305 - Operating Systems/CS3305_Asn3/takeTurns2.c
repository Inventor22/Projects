

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

/*
This is similar to the program developed for Part 1 except that either A or B could be
first. This program should be called takeTurns2.c which takes the same argument as
takeTurns1.c.
*/
#define A 0
#define B 1

sem_t sem[2];

struct arguments
{
    int numRuns;
};

void* threadA(void *argument)
{
    struct arguments *arg = argument;
    while (arg->numRuns-- > 0) {
        sem_wait(&sem[A]);
        printf("Thread A executing opA\n");
        sem_post(&sem[B]);
    }
    sem_destroy(&sem[A]);
    pthread_exit(NULL);
}

void* threadB(void* argument)
{
    struct arguments *arg = argument;
    while (arg->numRuns-- > 0) {
        sem_wait(&sem[B]);
        printf("Thread B executing opB\n");
        sem_post(&sem[A]);
    }
    sem_destroy(&sem[B]);
    pthread_exit(NULL);
}

void* (*threadArr[2]) (void*) = {&threadA, &threadB}; // function pointer array

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Must have 1 param");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL)); // seed random number generator

    pthread_t thread[2];

    struct arguments arg;
    arg.numRuns = 2*atoi(argv[1]);

    if (arg.numRuns < 0) {
        printf("Value must be positive.\n");
        exit(EXIT_FAILURE);
    }

    int order[2];
    order[0] = rand()&1; // random 0 or 1
    order[1] = !order[0]; // opposite of order[0]

    //printf("%d", order[0]);
    //printf("%d", order[1]);

    for (int i = 0; i < 2; i++) {
        if (sem_init(&sem[i], 0, order[i])) { // assign the randomly generated order
            printf(("Error creating semaphore.\n"));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 2; i++) {
        if (pthread_create(&thread[i], NULL,
            threadArr[i], (void *)&arg)) {
            fprintf(stderr, "Error while creating thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(thread[i], NULL);
    }

    return EXIT_SUCCESS;
}