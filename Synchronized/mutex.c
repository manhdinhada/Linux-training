/*
    Task 1: Count from 1 to 1 billion using multithreading with a mutex mechanism.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define mutex for thread synchronization
pthread_mutex_t count_mutex;
#define MAX 1000000000
#define NUM_THREAD 4

long range = MAX / NUM_THREAD;
long count = 0;

// Function executed by each thread
void *count_numbers_mutex(void *arg)
{
    long start = *(long *)arg;
    long local_count = 0;

    for (long j = start; j < start + range; j++)
    {
        local_count++;
    }

    pthread_mutex_lock(&count_mutex);

    count += local_count;

    pthread_mutex_unlock(&count_mutex);

    free(arg);
    pthread_exit(NULL);
}

int main()
{
    // Initialize mutex
    if (pthread_mutex_init(&count_mutex, NULL) != 0)
    {
        perror("Failed to initialize mutex");
        return 1;
    }

    pthread_t threads[NUM_THREAD];

    // Start timing the operation
    clock_t start = clock();

    // Create threads
    for (int i = 0; i < NUM_THREAD; i++)
    {
        long *start_index = malloc(sizeof(long));
        if (start_index == NULL)
        {
            perror("Failed to allocate memory");
            return 1;
        }

        *start_index = i * range;

        if (pthread_create(&threads[i], NULL, count_numbers_mutex, start_index) != 0)
        {
            perror("Failed to create thread");
            free(start_index);
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("Failed to join thread");
            return 1;
        }
    }

    // End timing the operation
    clock_t end = clock();
    double mutex_count_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Print the result
    printf("Multi-thread count: %ld, Timer: %.2f seconds\n", count, mutex_count_time);

    // Destroy the mutex
    if (pthread_mutex_destroy(&count_mutex) != 0)
    {
        perror("Failed to destroy mutex");
        return 1;
    }

    return 0;
}
