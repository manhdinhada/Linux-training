#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define MAX 1000000000
#define NUM_THREAD 4

long count = 0;
long range = MAX / NUM_THREAD;
sem_t count_semaphore;

void *count_numbers_semaphore(void *arg)
{
    long start = *(long *)arg;
    free(arg); // Free the allocated memory for start index

    long local_count = 0;
    for (long j = start; j < start + range; j++)
    {
        local_count++;
    }

    sem_wait(&count_semaphore);

    count += local_count;

    sem_post(&count_semaphore);

    pthread_exit(NULL);
}

int main()
{
    if (sem_init(&count_semaphore, 0, 1) != 0)
    {
        perror("sem_init failed");
        return 1;
    }

    pthread_t threads[NUM_THREAD];
    clock_t start = clock();

    for (int i = 0; i < NUM_THREAD; i++)
    {
        long *start_index = malloc(sizeof(long));
        if (start_index == NULL)
        {
            perror("malloc failed");
            return 1;
        }

        *start_index = i * range;

        if (pthread_create(&threads[i], NULL, count_numbers_semaphore, start_index) != 0)
        {
            perror("pthread_create failed");
            free(start_index);
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREAD; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            perror("pthread_join failed");
            return 1;
        }
    }

    clock_t end = clock();
    double semaphore_count_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Semaphore-thread count: %ld , Timer %.2f seconds\n", count, semaphore_count_time);

    if (sem_destroy(&count_semaphore) != 0)
    {
        perror("sem_destroy failed");
        return 1;
    }

    return 0;
}
