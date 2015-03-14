/* Program to compute Pi using Monte Carlo methods */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <sys/time.h>
#include <sched.h>

#define SEED 35791246

/*This function finds the difference between two times */

double computeElapsedTime(struct timeval start_time,
                          struct timeval finish_time)
{

    double start_count, end_count, elapsed_time;

    start_count =
        (double)start_time.tv_sec+1.e-6*(double)start_time.tv_usec;
    end_count =
        (double)finish_time.tv_sec+1.e-6*(double)finish_time.tv_usec;

    elapsed_time = (end_count - start_count);

    return elapsed_time;
}

int main(int argc, char **argv)
{
    int niter;
    int nFork;
    double x, y;
    int count = 0; /* # of points in the 1st quadrant of unit circle */
    double z;
    double pi;

    if (argc != 3)
    {
        perror("incorrect arguments\n");
        exit(1);
    }

    niter = atoi(argv[1]);
    nFork = atoi(argv[2]);

    /* initialize random numbers */
    for (int i = 0; i < nFork; i++)
    {
        if (fork() == 0)
        {
            int policy;
            struct sched_param param;

            // if i&1 == true (odd number) set SCHED_FIFO, else (even number) set SCHED_OTHER
            policy = (i&1) ? SCHED_FIFO : SCHED_OTHER;

            param.sched_priority = sched_get_priority_max(policy);

            if (sched_setscheduler(0, policy, &param))
            {
                perror("Error setting scheduler policy");
                exit(EXIT_FAILURE);
            }

            srand(SEED);
            count = 0;

            struct timeval start_time, finish_time;
            double elapsed_time;

            gettimeofday(&start_time, NULL);

            for (int j = 0; j<niter; j++)
            {
                x = (double)rand()/RAND_MAX;
                y = (double)rand()/RAND_MAX;
                z = x*x+y*y;
                if (z<=1) count++;
            }
            pi = (double)count/niter*4;

            gettimeofday(&finish_time, NULL);

            elapsed_time = computeElapsedTime(start_time, finish_time);

            printf("\nchild %d, sched = %s, trials= %d , aprx. pi = %g, time [s] = %f\n",
                   i,
                   (policy == SCHED_FIFO) ? "fifo" : "other",
                   niter, pi, elapsed_time);

            exit(EXIT_SUCCESS);
        }
    }
    printf("completed execution\n");
}
