#include <stdio.h>
#include <stdlib.h>
#include "headers.h"

void activities() {
    
    bg_process *temp = Head_bg->next;
    // iterate over the entries in list
    while (temp!=NULL) {
        
        if ( temp->pid > 0) {
            
                char pid_str[256];
                sprintf(pid_str, "%d", temp->pid);
                // Get the process state
                char stat_file[1024];
                snprintf(stat_file, sizeof(stat_file), "/proc/%s/stat", pid_str);

                FILE *stat_file_ptr = fopen(stat_file, "r");
                if (stat_file_ptr) {
                    char state;
                    fscanf(stat_file_ptr, "%*d %*s %c", &state);
                    fclose(stat_file_ptr);
                    const char *state_str;
                    
                    if(state == 'T')
                        state_str =  "Stopped";
                    else 
                        state_str =  "Running";
                     //don't print in activities if process killed

                    // Print the formatted entry
                    printf("[%d] : [%s] - [%s]\n", temp->pid, temp->command, state_str);
                }
            }
            temp=temp->next;
        }
    }

   



