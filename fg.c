#include "headers.h"

// Signal handler for SIGTTIN and SIGTTOU

int fg(char *argv[])
{

    pid_t pid = atoi(argv[1]);
    pid_t ppgid = getpgid(pid);
    if (pid <= 0)
    {
        printf(MAG);
        printf("Invalid PID\n");
        printf(COL_RESET);
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
        perror(MAG);
        perror("fg: kill");
        perror(COL_RESET);
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

    // reset the signal handlers for SIGTTIN and SIGTTOU
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // restore the terminal's foreground process group to the shell
    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1)
    {
        printf(MAG);
        perror("fg: tcsetpgrp");
        printf(COL_RESET);
        return 1;
    }

    return 0;
}

int bg(char *argv[])
{

    pid_t pid = atoi(argv[1]);
    pid_t ppgid = getpgid(pid);
    // check if the process with the given PID exists
    if (kill(pid, 0) == -1)
    {
        printf(MAG);
        printf("No such process found!\n");
        printf(COL_RESET);
        return 1;
    }

    // send the SIGCONT signal to resume the process
    if (kill(pid, SIGCONT) == -1)
    {
        printf(MAG);
        printf("Failed to resume process with PID %d.\n", pid);
        printf(COL_RESET);
        return 1;
    }

    // printf("Resumed process with PID %d.\n", pid);
    foreground_running_pid = getpid();
    return 0;
}
