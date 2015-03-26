/*
Author: Dustin Dobransky
id:     250575030
date:   23/03/15
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

/*
You are to write a program that synchronizes threads A and B such that these threads
take turns executing. Thread A should start first. The “operation” that thread A
executes consists of a print statement that prints “Thread A executing opA”. The
“operation” that thread B executes consists of a print statement that prints “Thread B
executing opB”.
The program should be called takeTurns1.c. This program takes one argument that
represents the number of times that thread A and thread B execute their operations.
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

    pthread_t thread[2];

    struct arguments arg;
    arg.numRuns = 2*atoi(argv[1]);
    if (arg.numRuns < 0) {
        printf("Value must be positive.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 2; i++) {
        if (sem_init(&sem[i], 0, i)) {
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