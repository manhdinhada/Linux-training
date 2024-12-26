#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
    int shm_fd;
    char *message = "Hello World\n";
    void *ptr;

    // Create or open shared memory with read-write permissions
    shm_fd = shm_open("shared_memory_example", O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1)
    {
        perror("Failed to open or create shared memory (shm_open)");
        return 1;
    }

    // Set the size of the shared memory
    if (ftruncate(shm_fd, 4096) == -1)
    {
        perror("Failed to set size of shared memory (ftruncate)");
        close(shm_fd);
        return 1;
    }

    // Map shared memory into the process's address space
    ptr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("Failed to map shared memory (mmap)");
        close(shm_fd);
        return 1;
    }

    // Write data to shared memory
    strcpy(ptr, message);

    // Unmap the shared memory
    if (munmap(ptr, 4096) == -1)
    {
        perror("Failed to unmap shared memory (munmap)");
    }

    // Close the shared memory file descriptor
    if (close(shm_fd) == -1)
    {
        perror("Failed to close shared memory file descriptor");
    }

    return 0;
}
