#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    printf("MAnhddddd0\n");
    pid_t p;
    p = fork();
    if (p < 0)
    {
        perror("fork fail!!!\n");
        exit(1);
    }

    else if (p == 0)
    {
        printf("Hello from child!\n");
        printf("pid child %d\n", getpid());
        printf("pid parent %d\n", getppid());
        exit(6);
    }
    else
    {
        printf("Hello from parent!\n");
        printf("pid parent %d\n", p);
        int status;
        pid_t child_pid = waitpid(p, &status, 0);

        if (WIFEXITED(status))
        {
            printf("child exited with status: %d\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status))
        {
            printf("child terminated by signal %d\n", WIFSIGNALED(status));
        }
        else
        {
            printf("fork failed.\n");
            exit(1);
        }
    }
    return 0;
}