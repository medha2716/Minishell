// #include "headers.h"
#include <stdio.h>


#include <unistd.h> // For geteuid()
#include <sys/types.h>
#include <pwd.h> // For getpwuid()
#include <limits.h>
#include <stdio.h> // For printf()
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dirent.h>   //for directory
#include <sys/stat.h> //for file status
#include <time.h>     //localtime
#include <grp.h>      //to get group



void print_process_info(int pid) {
    char stat_path[100];
    char status;
    int pgroup, vmemory;
    char executable_path[100];
    int back_fore=0;

    // Generate the path to the stat file for the given PID
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);

    // Open the stat file
    FILE *stat_file = fopen(stat_path, "r");
    if (!stat_file) {
        perror("Error opening stat file");
        return;
    }
   
    // Read the required information from the stat file
    char* stat_buf = calloc(100000, sizeof(char));
    assert(stat_buf != NULL);
    fread(stat_buf, 1, 100000, stat_file);

    fclose(stat_file);

    int no_of_tokens=0;
    char **tokens = malloc(40 * sizeof(char *)); // array of tokens

    char *delimiters = " ";
    char *token = strtok(stat_buf, delimiters);

    while (token != NULL)
    {
        tokens[no_of_tokens] = token;
        no_of_tokens++;

        token = strtok(NULL, delimiters);
    }

    tokens[no_of_tokens] = NULL;
    //take what we want
    status = tokens[2];
    pgroup = atoi(tokens[4]);  
    vmemory = atoi(tokens[22]);

    // Generate the path to the exe link for the given PID
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/exe", pid);

    // Read the executable path using the readlink function
    ssize_t len = readlink(stat_path, executable_path, sizeof(executable_path) - 1);
    if (len != -1) {
        executable_path[len] = '\0';
    } else {
        perror("Error reading executable path");
        return;
    }

    
    // 

    if (atoi(tokens[4]) == atoi(tokens[7]))
        back_fore = 43;
     // Get the foreground process group ID of the terminal
    pid_t terminal_foreground_pgid = tcgetpgrp(STDIN_FILENO);

    // Compare the PGID with the terminal's foreground process group ID
    if (pgroup == terminal_foreground_pgid) {
        printf("Foreground process\n"); // Foreground process
    } else {
        printf(" Background process\n"); // Background process
    }
    
    printf("PID: %d\n", pid);
    printf("Process Status: %c\n", status);
    printf("Process Group: %d\n", pgroup);
    printf("Virtual Memory: %d\n", vmemory);
    printf("Executable Path: %s\n", executable_path);

    //free stuff up
    free(stat_buf);
    free(tokens);
}

int main() {
    int pid = getpid(); // Replace with the PID you want to inspect
    print_process_info(pid);
    return 0;
}