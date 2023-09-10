#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

void activities() {
    
    bg_process *temp = Head_bg->next;
    // Iterate over the entries in /proc
    while (temp!=NULL) {
        // Check if the entry is a directory and its name is numeric
        if ( temp->pid > 0) {
            
                char pid_str[256];
                sprintf(pid_str, "%d", temp->pid);
                // Get the process state
                char stat_file[256];
                snprintf(stat_file, sizeof(stat_file), "/proc/%s/stat", pid_str);

                FILE *stat_file_ptr = fopen(stat_file, "r");
                if (stat_file_ptr) {
                    char state;
                    fscanf(stat_file_ptr, "%*d %*s %c", &state);
                    fclose(stat_file_ptr);
                    const char *state_str;
                    // Map the state code to "running" or "stopped"
                    if(state == 'R')
                        state_str =  "running";
                    else if(state == 'T')
                        state_str =  "stopped";
                    else
                        return; //don't print in activities if process killed

                    // Print the formatted entry
                    printf("[%d] : [%s] - [%s]\n", temp->pid, temp->command, state_str);
                }
            }
        }
    }

   



