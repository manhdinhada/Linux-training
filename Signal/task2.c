#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Function to print the current signal mask
void print_signal_mask()
{
    char sig_name[32]; // Buffer for signal name
    sigset_t set;
    int signum;

    // Get the current signal mask
    if (sigprocmask(SIG_BLOCK, NULL, &set) == -1)
    {
        perror("sigprocmask");
        return;
    }

    // Check and print each signal in the mask
    for (signum = 1; signum < NSIG; ++signum)
    {
        if (sigismember(&set, signum))
        {
            snprintf(sig_name, sizeof(sig_name), "Signal %d", signum);
            psignal(signum, sig_name);
        }
    }
}

int main()
{

    print_signal_mask();

    // Add SIGINT to the signal mask
    sigset_t new_set;
    sigemptyset(&new_set);       // Initialize an empty signal set
    sigaddset(&new_set, SIGINT); // Add SIGINT to the set

    // Block SIGINT
    if (sigprocmask(SIG_BLOCK, &new_set, NULL) == -1)
    {
        perror("sigprocmask");
        return 1;
    }

    printf("\nBlocking SIGINT successfully:\n");
    print_signal_mask();

    return 0;
}
