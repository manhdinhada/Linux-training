/*
    Task 1: Count from 1 to 1 billion using multithreading with each mechanism: semaphore, mutex, shared memory, and spinlock.

*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX 1000000000
#define NUM_THREAD 4
long range = MAX / NUM_THREAD;
long count = 0;

pthread_spinlock_t slock_count;

void *count_numbers_spinlock(void *arg)
{
    long start = *(long *)arg;
    long local_count = 0;
    for (long j = start; j < start + range; j++)
    {
        local_count++;
    }
    pthread_spin_lock(&slock_count);
    count += local_count;
    pthread_spin_unlock(&slock_count);
    free(arg);
    pthread_exit(NULL);
}

int main()
{

    pthread_spin_init(&slock_count, PTHREAD_PROCESS_PRIVATE);

    pthread_t threads[NUM_THREAD];
    clock_t start = clock();

    for (int i = 0; i < NUM_THREAD; i++)
    {
        long *start_index = malloc(sizeof(long));
        *start_index = i * range;

        if (pthread_create(&threads[i], NULL, count_numbers_spinlock, start_index))
        {
            printf("Failing to creat new thead!!!\n");
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREAD; i++)
    {
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();
    double spinlock_count_time = (end - start) / CLOCKS_PER_SEC;

    printf("Spin-thread count: %ld, Timer %.2f seconds\n", count, spinlock_count_time);
    pthread_spin_destroy(&slock_count);

    return 0;
}
