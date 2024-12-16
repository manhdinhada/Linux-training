/*
    Write a C program to block SIGINT, then check if SIGINT is in the pending signals, and if so, exit the program
*/

#include <stdio.h>
#include <signal.h>

int main()
{
    sigset_t my_sig_set;
    sigset_t my_pending_set;

    sigemptyset(&my_sig_set);
    sigaddset(&my_sig_set, SIGINT);
    sigprocmask(SIG_BLOCK, &my_sig_set, NULL);

    if (sigprocmask(SIG_BLOCK, NULL, &my_sig_set) == -1)
    {
        perror("sigpromask");
        return 1;
    }
    printf("SIGINT is blocked. PLease enter Ctrl+C to send SIGINT.\n");

    // check if pending
    while (1)
    {
        if (sigpending(&my_pending_set) == -1)
        {
            perror("Failed to retrieve pending signals");
            return 1;
        }

        // Check if SIGINT is pending
        if (sigismember(&my_pending_set, SIGINT))
        {
            printf("SIGINT is pending!\n");
            return 0;
        }
    }

    return 0;
}