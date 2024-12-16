/**
 * Write a program that create a child process, then both parent and child will
 * write to the file. Then child process will be changed to the new process by
 * using execl(). The child process now will print the content of the file that
 * written before (open_file).
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// Function to write a string to a file, character by character
void write_string_to_file(int pid, int fd, char string[])
{
    ssize_t byte_written;

    // Iterate through each character in the string
    for (int i = 0; i < strlen(string); i++)
    {
        byte_written = write(fd, &string[i], 1);

        // Check if write operation succeeded
        if (byte_written != 1)
        {
            if (pid == 0)
            {
                printf("Parent write failed!\n");
            }
            else
            {
                printf("Child write failed!\n");
            }
            return;
        }
    }
}

int main(int argc, char **argv)
{
    char parent_string[] = "Parent"; // String to be written by parent process
    char child_string[] = "Child";   // String to be written by child process

    pid_t pid = getpid(); // Get the current process ID

    // Validate the number of arguments
    if (argc <= 1)
    {
        printf("Missing arguments!\n");
        return -1;
    }
    else if (argc > 2)
    {
        printf("Too many arguments!\n");
        return -1;
    }

    printf("Parent PID: %d\n", pid);

    // Open the file in write-only mode and truncate it if it exists
    int fd = open(argv[1], O_WRONLY | O_TRUNC);
    if (fd == -1)
    {
        printf("Open failed!\n");
        return -1;
    }

    // Create a child process using fork()
    int p = fork();
    if (p == 0)
    {
        // Child process writes its string to the file
        write_string_to_file(p, fd, child_string);

        // Replace the child process with a new program using execl()
        if (execl("open_file", "open_file", argv[1], NULL) == -1)
        {
            printf("Switch task failed!\n");
        }
    }
    else
    {
        // Parent process writes its string to the file
        printf("parent process in here!\n");
        write_string_to_file(p, fd, parent_string);
    }

    // Close the file descriptor
    close(fd);
    return 0;
}
