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



void print_process_info(char* pid) {
    
    // int pid;
    // char *pid_str = (char*)malloc(sizeof(char)*strlen(buf));
    // if(buf[9]=='\0')
    // {
    //     pid_str = "self";
    // }
    // else{
    //    
    // }
    
    // char spath[256];
    // snprintf(spath, sizeof(spath), "/proc/%s/status", pid_str);

    // FILE *f = fopen(spath, "r");
    // if (!f)
    // {
    //     printf("\033[0;31m");
    //     perror("Failed to open status file");
    //     printf("\x1b[0m");
    //     return 0;
    // }
    // if (strcmp(pid_str, "self") != 0)
    //     {printf("pid: %s\n", pid_str);
    //      pid = atoi(pid_str); }
    // else
    // {
    //     int pid2 = getpid();
    //     printf("pid: %d\n", pid2);
    //     pid=pid2;
    // }
    // char line[256];
    // char pstatus;
    // int grp;
    // unsigned long mem;
    // char *epath = (char *)malloc(sizeof(char) * 1024);
    // int bgflag=0;
    // int status;
    // int result = waitpid(pid, &status, 0x01);

    // if (result == -1) 
    // {
    //     printf("\033[0;31m");
    //     perror("waitpid");
    //     printf("\x1b[0m");
    //    // exit(EXIT_FAILURE);
    // } else if (result == 0) 
    // {
    //     bgflag=1;
    // }

    // while (fgets(line, sizeof(line), f))
    // {
        
    //     if (strncmp(line, "PPid:", 5) == 0)
    //     {
    //         sscanf(line, "PPid:\t%d", &grp);
    //     }
    //     else if (strncmp(line, "Name:", 5) == 0)
    //     {
    //         sscanf(line, "Name:\t%s", epath);
    //     }
    //     else if (strncmp(line, "State:", 6) == 0)
    //     {
    //         sscanf(line, "State:\t%c", &pstatus);
    //     }
    //     else if (strncmp(line, "VmSize:", 7) == 0)
    //     {
    //         sscanf(line, "VmSize:\t%lu", &mem);
    //     }
    // }
    // if(bgflag==0)
    // printf("Process status: %c+\n", pstatus);
    // else
    // printf("Process status: %c\n", pstatus);
    // printf("Process group: %d\n", grp);
    // printf("Virtual memory: %lu KB\n", mem);
    // printf("executable path: %s\n", epath);

    // fclose(f);
    // return 1;
    char stat_path[100];
    char status[1000];
    int pgroup, vmemory;
    char executable_path[100];
    int back_fore=0;

    //  int ind=0;
    //     for(int i=9;i<strlen(buf)-1;i++)
    //     pid_str[ind++]=(char)buf[i];
    //     pid_str[ind]='\0';
    // Generate the path to the stat file for the given PID
    snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat",pid);

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

    printf("%s\n",stat_buf);
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
    strcpy(status , tokens[2]);
    pgroup = atoi(tokens[4]);  
    vmemory = atoi(tokens[22]);

    // Generate the path to the exe link for the given PID
    snprintf(stat_path, sizeof(stat_path), "/proc/%s/exe",pid);

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
        printf("Foreground process\n"); 
     // Get the foreground process group ID of the terminal
    pid_t terminal_foreground_pgid = tcgetpgrp(STDIN_FILENO);

    // Compare the PGID with the terminal's foreground process group ID
    if (pgroup == terminal_foreground_pgid) {
        printf("Foreground process\n"); // Foreground process
    } else {
        printf(" Background process\n"); // Background process
    }
    
    // printf("PID: %d\n", pid);
    printf("Process Status: %s\n", status);
    printf("Process Group: %d\n", pgroup);
    printf("Virtual Memory: %d\n", vmemory);
    printf("Executable Path: %s\n", executable_path);

    //free stuff up
    free(stat_buf);
    free(tokens);
}

int main() {
    // int pid = getpid(); // Replace with the PID you want to inspect
    // print_process_info(pid);

    char pid_now[]="12674";
     print_process_info(pid_now);
    return 0;
}