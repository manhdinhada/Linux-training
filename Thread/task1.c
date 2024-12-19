#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000000000
#define NUM_THREADS 4
long range = MAX / NUM_THREADS;
long count = 0;
pthread_mutex_t count_mutex;

long start;
// Function to count odd numbers in a specified range
void *count_odd_numbers(void *arg)
{
    long start = *(long *)arg; // Retrieve the value from the pointer
    long local_count = 0;

    for (long i = start; i <= start + range; i++)
    {
        if (i % 2 == 1)
        {
            local_count++;
        }
    }

    pthread_mutex_lock(&count_mutex);
    count += local_count;
    pthread_mutex_unlock(&count_mutex);

    free(arg);
    pthread_exit(NULL);
}

int main()
{
    // Initialize the mutex
    pthread_mutex_init(&count_mutex, NULL);

    // Count odd numbers without using multithreading
    clock_t start = clock();

    long single_thread_count = 0;
    for (long i = 1; i <= MAX; i += 2)
    {
        single_thread_count++;
    }

    clock_t end = clock();
    double single_thread_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Single-thread count: %ld, Time: %.2f seconds\n", single_thread_count, single_thread_time);

    // Count odd numbers using multithreading
    pthread_t threads[NUM_THREADS];
    start = clock();

    for (int i = 0; i < NUM_THREADS; i++)
    {
        long *start_index = malloc(sizeof(long));
        *start_index = i * range + 1;

        if (pthread_create(&threads[i], NULL, count_odd_numbers, start_index))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    end = clock();
    double multi_thread_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Multi-thread count: %ld, Time: %.2f seconds\n", count, multi_thread_time);

    // Destroy the mutex
    pthread_mutex_destroy(&count_mutex);

    return 0;
}
