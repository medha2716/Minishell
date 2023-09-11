#include "headers.h"

// Signal handler for SIGTTIN and SIGTTOU

int fg(char *argv[])
{

    pid_t pid = atoi(argv[1]);
    pid_t ppgid = getpgid(pid);
    if (pid <= 0)
    {
        fprintf(stderr, "Invalid PID\n");
        return 1;
    }

    if (tcsetpgrp(STDIN_FILENO, ppgid) == -1)
    {
        printf(MAG);
        printf("No such process found!\n");
        printf(COL_RESET);
        return 1;
    }
    // Use tcsetpgrp to set the terminal's foreground process group to the specified PID
    // if (tcsetpgrp(STDIN_FILENO, pid) == -1) {
    //     perror("tcsetpgrp");
    //     return 1;
    // }

    // Send a SIGCONT signal to the process to resume it (if it's stopped)
    if (kill(pid, SIGCONT) == -1)
    {
        perror("kill");
        return 1;
    }

    foreground_running_pid = getpid(); // or do fore...=pid;

    // printf("%d",fgppid);

    // Wait for the process to finish (or be stopped)

    int status;
    if (waitpid(-ppgid, &status, WUNTRACED) == -1)
    {
        printf(MAG);
        printf("No such process found!\n");
        printf(COL_RESET);
        return 1;
    }

    // Check if the process was stopped
    // if (WIFSTOPPED(status))
    // {
    //     printf("Background process with PID %d was stopped.\n", pid);
    // }
    // else
    // {
    //     printf("Background process with PID %d has finished.\n", pid);
    // }

    // Reset the signal handlers for SIGTTIN and SIGTTOU
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // Restore the terminal's foreground process group to the shell
    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
    {
        perror("tcsetpgrp");
        return 1;
    }

    return 0;
}

int bg(char *argv[])
{
    // // Check if a PID is provided as a command line argument
    // if (argc != 2) {
    //     fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
    //     return 1;
    // }

    // Convert the PID argument to an integer
    pid_t pid = atoi(argv[1]);
    pid_t ppgid = getpgid(pid);
    // Check if the process with the given PID exists
    if (kill(pid, 0) == -1)
    {
        perror("Error");
        fprintf(stderr, "No such process found with PID %d.\n", pid);
        return 1;
    }

    // Send the SIGCONT signal to resume the process
    if (kill(pid, SIGCONT) == -1)
    {
        perror("Error");
        fprintf(stderr, "Failed to resume process with PID %d.\n", pid);
        return 1;
    }

    // printf("Resumed process with PID %d.\n", pid);
    foreground_running_pid = getpid();
    return 0;
}
