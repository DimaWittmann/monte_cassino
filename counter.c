#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static const unsigned long nthreads = 4;
static const unsigned long ncounts = 10000000;

// Trivial counter implementation

static unsigned long counter = 0;
static unsigned int barrier = 0;

void lock(unsigned int* barrier)
{
    if (barrier) {
        while(__sync_fetch_and_or(barrier, 1))
            ;    
    }
}

void unlock(unsigned int* barrier)
{
    if (barrier) {
        *barrier = 0;
    }
}

void counter_inc()
{
    counter++;
}

unsigned long read_counter()
{
    return counter;
}

// Thread routine

void *threadfn(void *arg)
{
    int i;

    for (i = 0; i < ncounts; i++) {
        lock(&barrier);
        counter_inc();
        unlock(&barrier);
    }
    return 0;
}

int main()
{
    int i;

    pthread_t pids[nthreads];
    for (i = 0; i < nthreads; i++) {
        int r = pthread_create(&pids[i], NULL, threadfn, NULL);
        if (r != 0) {
            perror("pthread_create: ");
            exit(1);
        }
    }
    for (i = 0; i < nthreads; i++) {
        pthread_join(pids[i], NULL);
    }
   
    printf("Expected counter value is: %lu\n", nthreads * ncounts);
    printf("Real counter value is: %lu\n", read_counter());
   
    return 0;
}
