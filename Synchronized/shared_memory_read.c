#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
    int shm_fd;
    char read_buffer[1024];
    void *ptr = NULL;

    // Open shared memory with read-only permissions (change O_RDONLY to O_RDWR if needed)
    shm_fd = shm_open("shared_memory_example", O_RDONLY, 0666);
    if (shm_fd == -1)
    {
        perror("Failed to open shared memory (shm_open)");
        return 1;
    }

    // Map shared memory into the process's address space
    ptr = mmap(0, 4096, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED)
    {
        perror("Failed to map shared memory (mmap)");
        if (shm_unlink("shared_memory_example") == -1)
        {
            perror("Failed to unlink shared memory");
        }
        return 1;
    }

    // Read data from shared memory
    printf("Data read from shared memory: %s\n", (char *)ptr);

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

    // Unlink the shared memory to clean up
    if (shm_unlink("shared_memory_example") == -1)
    {
        perror("Failed to unlink shared memory");
    }

    return 0;
}
