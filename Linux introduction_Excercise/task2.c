#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char ch;

    // Open the file in read mode
    fd = open("task2.txt", O_RDONLY);
    if (fd == -1)
    {
        printf("Error!!\n");
        return EXIT_FAILURE;
    }

    // Read and print the content of the file
    while (read(fd, &ch, 1) > 0)
    {
        putchar(ch); // Print each character
    }

    // Close the file
    close(fd);

    return EXIT_SUCCESS;
}
