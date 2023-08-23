#ifndef HEADERS_H_
#define HEADERS_H_

#include <unistd.h> // For geteuid()
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h> //for kill
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
#include "pastevents.h"
#include "prompt.h"

#define HOSTNAME_MAX 512
#define PATHno_of_tokens_MAX 100000
#define MAX_HIST_SIZE 20
#define HIST_DISPLAY 10


// Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define GREY "\x1B[90m"
#define COL_RESET "\x1B[0m"

typedef struct bg_process
{
    int pid;
    struct bg_process *next;
    struct bg_process *prev;
    int status;
    char *command;
} bg_process;

extern char *OLDPWD;
extern int no_of_bg ;
extern bg_process *Head_bg;
extern char HOME[1024];



void proclore(char *pid);


int compare_entries(const void *a, const void *b);
void display_permissions(mode_t mode);
void print_name(char *name, struct stat file_stat);
int peek_print(const char *path, int show_hidden, int show_details);
int peek(char **args);


void print_path();
void warp(char **args);


int is_match(const char *name, const char *search_term);
void search_directory(int only_files,int only_dir,const int target,const char *dir_path, const char *search_term);
int seek( char *args[]);


char *sh_read_line();
char **sh_split_line(char *line);
char **sh_extract_commands(char *line);
int foreground(char **args);
void add_to_list_bg(bg_process *new);
int background(char **args);
void remove_bg_list(bg_process *temp);
void check_bg_if_ended();
void sh_exec(char **args,char* line_execute_pastevnts);

#endif