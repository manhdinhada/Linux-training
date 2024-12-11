#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    // Check if file argument is provided
    if (argc < 2)
    {
        printf("File wasn't provided!!\n");
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening file!!\n");
        return -1;
    }

    char ch;
    while (read(fd, &ch, 1) > 0)
    {
        putchar(ch);
    }

    // Close the file descriptor after reading
    close(fd);

    return 0;
}
