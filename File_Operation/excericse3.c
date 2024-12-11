#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <linux/falloc.h>

#define BUF_SIZE 4096

int main(int argc, char *argv[])
{
    int src_fd, dest_fd;
    ssize_t num_read;
    char buffer[BUF_SIZE];

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source file> <destination file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open source file
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1)
    {
        perror("Error opening source file");
        return -1;
    }

    // Create destination file
    dest_fd = open(argv[2], O_WRONLY | O_CREAT);
    if (dest_fd == -1)
    {
        perror("Error opening destination file");
        return -1;
    }

    // function read and write return number of byte data that write or read
    // "while": because each read will read in the number of data (BUF_SIZE = 4096), and then if having other data remaining will continue to read
    while ((num_read = read(src_fd, buffer, BUF_SIZE)) > 0)
    {
        for (int i = 0; i < num_read; i++)
        {
            if (buffer[i] != '\0')
            {
                if (write(dest_fd, &buffer[i], 1) != 1)
                {
                    printf(" Can't write to destination buffer!!\n");
                }
            }
            else
            {
                off_t position = lseek(dest_fd, 0, SEEK_CUR);
                fallocate(dest_fd, 0, position, 1);
            }
        }
    }

    // Close files
    close(src_fd);
    close(dest_fd);
}
