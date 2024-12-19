#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // For sleep()

typedef struct
{
    char full_name[30];
    int born_year;
    char phone_number[12];
    char home_town[30];
} thread_t;

// Thread function for printing verification every 2 seconds
void *thread_1_verify(void *arg)
{
    while (1)
    {
        printf("\nVerification: Thread information is running now!\n");
        sleep(2);
    }
    pthread_exit(NULL);
}

// Thread function for printing human information
void *thread_information(void *arg)
{
    thread_t *thread_data = (thread_t *)arg;
    printf("-------------------------------------------\n");
    printf("\nThread is running...\n");
    printf("Full name: %s\n", thread_data->full_name);
    printf("Born year: %d\n", thread_data->born_year);
    printf("Phone number: %s\n", thread_data->phone_number);
    printf("Hometown: %s\n", thread_data->home_town);
    printf("-------------------------------------------\n");

    pthread_exit(NULL);
}

int main()
{
    pthread_t thread1, thread2;

    thread_t data_thread2 = {
        "ManhDN4",
        2002,
        "0359574599",
        "Hanoi"};

    // Create threads
    pthread_create(&thread1, NULL, thread_1_verify, NULL);
    pthread_create(&thread2, NULL, thread_information, &data_thread2);

    // Let the threads run for some time
    sleep(10); // Run for 10 seconds before canceling threads

    // Cancel and join threads to ensure graceful termination
    pthread_cancel(thread1);
    pthread_cancel(thread2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("\nMain program terminated.\n");
    return 0;
}
