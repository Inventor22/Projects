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
This is similar to the program developed for Part 1 except that there are N processes
This program should be called takeTurns3.c which takes as arguments the number of
times that the threads execute their operation and the number of threads.
*/

sem_t* sem; // global pointer to semaphores

typedef struct
{
    int numRuns;
    int idCurrent, idNext;
} arguments;

void* threadFunc(void *argument)
{
    arguments* arg = (arguments*) argument;
    while (arg->numRuns-- > 0) {
        sem_wait(&sem[arg->idCurrent]);
        printf("Thread %d executing its op\n", arg->idCurrent);
        sem_post(&sem[arg->idNext]);
    }
    //sem_destroy(&sem[arg->idCurrent]);
    pthread_exit(NULL);
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        printf("Must have 2 params");
        exit(EXIT_FAILURE);
    }

    const int numThreads = atoi(argv[1]);
    const int numRuns    = atoi(argv[2]);

    if (numThreads <= 0) {
        printf("Value must be positive.\n");
        exit(EXIT_FAILURE);
    }

    // dynamically allocate threads, arguments, and semaphores
    pthread_t* thread     = (pthread_t*) malloc(sizeof(pthread_t) * numThreads);
    arguments* arg = (arguments*) malloc(sizeof(arguments) * numThreads);
    sem                   = (sem_t*) malloc(sizeof(sem_t)     * numThreads);

    // assign arguments
    for (int i = 0; i < numThreads; i++) {
        arg[i].idCurrent = i;
        arg[i].idNext = i+1;
        arg[i].numRuns = numRuns;

        // initialize semaphores
        if (sem_init(&sem[i], 0, i==0)) { // set first semaphore to 1, rest 0
            printf(("Error creating semaphore.\n"));
            exit(EXIT_FAILURE);
        }
    }
    arg[numThreads-1].idNext = 0; // wrap around

    for (int i = 0; i < numThreads; i++) { // create threads
        if (pthread_create(&thread[i], NULL,
            threadFunc, (void*) &arg[i])) {
                fprintf(stderr, "Error while creating thread\n");
                exit(1);
        }
    }

    for (int i = 0; i < numThreads; i++) // join threads
        pthread_join(thread[i], NULL);

    free(thread); free(sem); free(arg);
    return EXIT_SUCCESS;
}