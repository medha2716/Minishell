#include <stdio.h>
#include <stdlib.h>
#include "headers.h"


void activities() {
    bg_process *temp = Head_bg->next;
    int num_processes = 0;
    
    // count the number of processes
    while (temp != NULL) {
        num_processes++;
        temp = temp->next;
    }
    if(num_processes==0)
        return;
    // allocate an array of bg_process pointers
    bg_process **process_array = (bg_process **)malloc(num_processes * sizeof(bg_process *));


    temp = Head_bg->next;
    int i = 0;
    while (temp != NULL) {
        process_array[i] = temp;
        temp = temp->next;
        i++;
    }

    // Sort the process_array based on PID in lexicographic order
    for (int j = 0; j < num_processes - 1; j++) {
        for (int k = j + 1; k < num_processes; k++) {
            if (process_array[j]->pid > process_array[k]->pid) {
                bg_process *temp_swap = process_array[j];
                process_array[j] = process_array[k];
                process_array[k] = temp_swap;
            }
        }
    }

    // iterate over the sorted process_array and print the entries
    for (int j = 0; j < num_processes; j++) {
        temp = process_array[j];
        if (temp->pid > 0) {
            char pid_str[256];
            sprintf(pid_str, "%d", temp->pid);
            
            char stat_file[1024];
            snprintf(stat_file, sizeof(stat_file), "/proc/%s/stat", pid_str);

            FILE *stat_file_ptr = fopen(stat_file, "r");
            if (stat_file_ptr) {
                char state;
                fscanf(stat_file_ptr, "%*d %*s %c", &state);
                fclose(stat_file_ptr);
                const char *state_str;

                if (state == 'T')
                    state_str = "Stopped";
                else
                    state_str = "Running"; //don't print in activities if process killed

                // print the formatted entry
                printf("[%d] : [%s] - [%s]\n", temp->pid, temp->command, state_str);
            }
        }
    }

    // free the allocated memory
    free(process_array);
}



