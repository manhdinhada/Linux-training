#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    int x = 0, y = 0;
    char buffer[3];

    fd = open("/dev/input/mice", O_RDONLY);
    if (fd == -1)
    {
        fprintf(stderr, "Unable to open mice!!!\n");
        return -1;
    }

    while (1)
    {
        int bytes = read(fd, buffer, sizeof(buffer)); // Read 3 bytes at a time
        if (bytes > 0)
        {
            // x and y represent mouse movement deltas.
            x += (char)buffer[1];
            y += (char)buffer[2];
            printf("Mouse vertical: %d and horizontal: %d\n", x, y);
        }
        else
        {
            printf("Error reading mouse input.\n");
            break;
        }
    }

    close(fd); // Use close() instead of fclose() for file descriptors
    return 0;
}
