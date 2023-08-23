#ifndef __PEEK_H
#define __PEEK_H

int compare_entries(const void *a, const void *b);
void display_permissions(mode_t mode);
void print_name(char *name, struct stat file_stat);
int peek_print(const char *path, int show_hidden, int show_details);
int peek(char **args);

#endif