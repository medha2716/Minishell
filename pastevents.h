#ifndef __PASTEVNTS_H
#define __PASTEVNTS_H

#define BOOKKEEP_FILE_PATH "bookkeep.txt"
#define TEMP_FILE_PATH "temp.txt"

int check_if_pastevents(char* line);
void remove_oldest_command();
int add_command(char *command);
int pastevents_purge();

int execute_command(int command_position,char* to_be_added);
int read_command();

#endif