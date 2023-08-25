#include "headers.h"

void proclore(char *pid)
{

    char stat_path[PATH_MAX];
    char status[PATH_MAX];
    int pgroup, vmemory;
    char executable_path[PATH_MAX];

    snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", pid);

    // Open the stat file
    FILE *stat_file = fopen(stat_path, "r");
    if (!stat_file)
    {
        perror(MAG);
        perror("Error opening stat file");
        perror(COL_RESET);
        return;
    }

    // Read the required information from the stat file
    char *stat_buf = calloc(100001, sizeof(char));
    assert(stat_buf != NULL);
    fread(stat_buf, 1, 100000, stat_file);

    fclose(stat_file);

    // printf("%s\n", stat_buf);
    int no_of_tokens = 0;
    char **tokens = malloc(40 * sizeof(char *)); // array of tokens
    int tokens_array_size=40;
    char *delimiters = " ";
    char *token = strtok(stat_buf, delimiters);

    while (token != NULL)
    {
        tokens[no_of_tokens] = token;
        no_of_tokens++;

           if (no_of_tokens >= tokens_array_size)
        {
            tokens_array_size = (int)(tokens_array_size * 2);
            tokens = realloc(tokens, tokens_array_size * sizeof(char *));
        }

        token = strtok(NULL, delimiters);
    }

    tokens[no_of_tokens] = NULL;
    // take what we want
    strcpy(status, tokens[2]);
    pgroup = atoi(tokens[4]);
    vmemory = atoi(tokens[22]);
    int tty_nr=atoi(tokens[6]); //contolling terminal's minor device number
    // Generate the path to the exe link for the given PID
    snprintf(stat_path, sizeof(stat_path), "/proc/%s/exe", pid);

    // Read the executable path using the readlink function
    ssize_t len = readlink(stat_path, executable_path, sizeof(executable_path) - 1);
    if (len != -1)
    {
        executable_path[len] = '\0';
    }
    else
    {
        perror(MAG);
        perror("Error reading executable path");
        perror(COL_RESET);
        return;
    }

    //
   


    
    int pid_print;
    if (strcmp(pid, "self") == 0)
    {
        pid_print = getpid();
    }
    else
    {
        pid_print = atoi(pid);
    }

    printf("pid : %d\n", pid_print);
    printf("Process Group : %d\n", pgroup);

    int terminal_fg_pgid=tcgetpgrp(STDIN_FILENO);
    if (pgroup==terminal_fg_pgid) //background processes are not attached to any terminal
    {
       printf("process status : %s+\n", status); // Foreground process
    }
    else
    {
        printf("process status : %s\n", status); // Background process
    }
    
    
    printf("Virtual memory : %d\n", vmemory);

     if (strncmp(executable_path, HOME, strlen(HOME)) == 0)
    {
        printf("executable path : ~%s\n", executable_path+ strlen(HOME));
    }
    else
    {
        printf("executable path : %s\n", executable_path);
    }
    
 
    // free stuff up

   free(tokens);
   free(stat_buf);
}
   

// int main() {
//     // int pid = getpid(); // Replace with the PID you want to inspect
//     // print_process_info(pid);

//     char pid_now[]="12674";

//     //  print_process_info(pid_now);
//      printf("%d\n",pid);
//     return 0;

//       //  int ind=0;
//     //     for(int i=9;i<strlen(buf)-1;i++)
//     //     pid_str[ind++]=(char)buf[i];
//     //     pid_str[ind]='\0';
//     // Generate the path to the stat file for the given PID
// }