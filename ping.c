#include "headers.h"
void ignore_promptprint()
{
    printf("\n");
    prompt();
    fflush(stdout);
    return;
}
int ping(char **args)
{
    if (args[1] == NULL)
    {
        printf(MAG);
        printf("ping: usage: ping <pid> <signal_number>\n");
        printf(COL_RESET);
        return 1;
    }
    if (args[2] == NULL)
    {
        printf(MAG);
        printf("ping: usage: ping <pid> <signal_number>\n");
        printf(COL_RESET);
        return 1;
    }
    if (atoi(args[1]) == 0)
    {
        printf(MAG);
        printf("ping: invalid pid\n");
        printf(COL_RESET);
        return 1;
    }
    if (atoi(args[2]) == 0)
    {
        printf(MAG);
        printf("ping: invalid signal number\n");
        printf(COL_RESET);
        return 1;
    }
    if (kill(atoi(args[1]), 0) == -1)
    {
        perror(MAG);
        fprintf(stderr, "No such process found with PID %d.\n", atoi(args[1]));
        perror(COL_RESET);
        return 1;
    }
    if (kill(atoi(args[1]), (atoi(args[2]) % 32)) == -1)
    {
        perror(MAG);
        perror("ping: ");
        perror(COL_RESET);
        return 1;
    }

    int status;
    waitpid(atoi(args[1]),&status,WNOHANG);
    if(WIFSTOPPED(status))
        printf("whoa\n");
    return 0;
}

void kill_bg()
{
    bg_process *temp = Head_bg->next;

    while (temp != NULL)
    {

        if (temp->pid > 0)
        {
            if (kill(temp->pid, 9) == -1)
            {
                printf(MAG);
                printf("Couldn't kill process with pid: %d",temp->pid);
                printf(COL_RESET);
                return;
            }
        }
        temp = temp->next;
    }
}