#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int input_ID;
    char path[256]; // Buffer to hold the file path
    char ch;

    printf("Enter a process ID: ");
    if (scanf("%d", &input_ID) != 1)
    {
        printf("Invalid input. Please enter a valid process ID.\n");
        return EXIT_FAILURE;
    }

    snprintf(path, sizeof(path), "/proc/%d/comm", input_ID);

    int fd = open(path, O_RDONLY);
    if (fd == -1)
    {
        printf("Error opening proc file for PID %d\n", input_ID);
        return EXIT_FAILURE;
    }

    printf("The name of the process with PID %d is:\n", input_ID);
    while (read(fd, &ch, 1) > 0)
    {
        putchar(ch); // Output each character from the process name
    }

    close(fd);

    return EXIT_SUCCESS;
}
