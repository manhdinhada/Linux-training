#include <sys/un.h>
#include <sys/socket.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define BUF_SIZE 10               /* Maximum size of messages exchanged between client and server */
#define SV_SOCK_PATH "./ud_ucase" /* Server socket file path */

ssize_t numBytes;
size_t msgLen;
int sfd;
char resp[BUF_SIZE];
struct sockaddr_un svaddr, claddr;

/* Thread function to receive data from the server */
void *client_receive_data(void *arg)
{
    while (1)
    {
        /* Receive response from the server */
        numBytes = recvfrom(sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (numBytes == -1)
        {
            perror("recvfrom error");
            continue;
        }

        /* Print the received response */
        printf("Received response data: %s\n", resp);

        /* Clear the buffer for the next response */
        memset(resp, 0, BUF_SIZE);
    }
    return NULL;
}

int main()
{
    pthread_t thread_receive;
    char *data = (char *)malloc(100 * sizeof(char)); // Allocate memory for input data
    if (data == NULL)
    {
        perror("malloc error");
        return -1;
    }

    /* Create a UNIX domain datagram socket */
    sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1)
    {
        perror("socket error");
        free(data);
        return -1;
    }

    /* Set up the client socket address */
    memset(&claddr, 0, sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path, sizeof(claddr.sun_path), "./ud_ucase_cl_%d", getpid()); // Unique client socket path

    /* Bind the client socket to the specified address */
    if (bind(sfd, (struct sockaddr *)&claddr, sizeof(struct sockaddr_un)) == -1)
    {
        perror("bind error");
        free(data);
        close(sfd);
        return -1;
    }

    /* Construct the server socket address */
    memset(&svaddr, 0, sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path, SV_SOCK_PATH, sizeof(svaddr.sun_path) - 1);

    /* Create a thread to handle data receiving */
    if (pthread_create(&thread_receive, NULL, client_receive_data, NULL) != 0)
    {
        perror("pthread_create error");
        free(data);
        close(sfd);
        return -1;
    }

    while (1)
    {
        /* Read input data from the user */
        if (scanf("%s", data) != 1)
        {
            perror("scanf error");
            continue;
        }

        msgLen = strlen(data); /* Length of the input message */

        /* Send the message to the server */
        if (sendto(sfd, data, msgLen, 0, (struct sockaddr *)&svaddr, sizeof(struct sockaddr_un)) != msgLen)
        {
            perror("sendto error");
            continue;
        }

        printf("Sent data: %s\n", data);
    }

    /* Cleanup resources */
    remove(claddr.sun_path);
    free(data);
    close(sfd);
    return 0;
}
