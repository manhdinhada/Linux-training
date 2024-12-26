/*
Task 1: Write a chat program based on the client-server model. Both should run on the same computer.
*/

#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define SV_SOCK_PATH "./ud_ucase" // Path for the UNIX domain socket

// Global variables
socklen_t len;
ssize_t numBytes;
int sockfd = -1;
struct sockaddr_un server_addr, client_addr;
char recv_buffer[1024];
bool flag = false;

// Function to send data from the server to the client
void *server_send_data(void *arg)
{
    for (;;)
    {
        if (flag == true)
        {
            printf("Send response data:  %s\n", recv_buffer);

            // Send the data back to the client
            if (sendto(sockfd, recv_buffer, numBytes, 0, (struct sockaddr *)&client_addr, len) != numBytes)
            {
                perror("sendto error");
            }

            memset(recv_buffer, 0, 1024);
            flag = false;
        }
    }
    return NULL;
}

void main()
{
    pthread_t thread_send;
    memset(recv_buffer, 0, sizeof(recv_buffer));
    memset(&server_addr, 0, sizeof(server_addr));

    // Create a socket
    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("socket error");
        return;
    }

    // Remove the socket file if it already exists
    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
    {
        perror("remove error");
        close(sockfd);
        return;
    }

    // Configure the server address
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SV_SOCK_PATH, sizeof(server_addr.sun_path) - 1);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind error");
        close(sockfd);
        return;
    }

    // Create a thread for sending data
    if (pthread_create(&thread_send, NULL, server_send_data, NULL) != 0)
    {
        perror("pthread_create error");
        close(sockfd);
        return;
    }

    // Server loop to receive data from the client
    for (;;)
    {
        len = sizeof(struct sockaddr_un);

        // Receive data from the client
        numBytes = recvfrom(sockfd, recv_buffer, 1024, 0, (struct sockaddr *)&client_addr, &len);
        if (numBytes == -1)
        {
            perror("recvfrom error");
            continue;
        }

        printf("Receive data: %s\n", recv_buffer);
        flag = true;
    }

    close(sockfd);
}
