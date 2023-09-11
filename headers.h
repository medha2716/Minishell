#ifndef HEADERS_H_
#define HEADERS_H_

#include <unistd.h> // For geteuid()
#include <sys/types.h>
#include <fcntl.h>
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
#include "peek.h"
#include "seek.h"
#include "warp.h"
#include "proclore.h"

#define HOSTNAME_MAX 512


// Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;35m"
#define BLU "\e[0;34m"
#define MAG "\e[0;33m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"
#define GREY "\x1B[90m"
#define PNK "\x1b[38;5;206m"
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
extern long time_flag;
extern char arg_0[1024];
extern int foreground_running_pid;
extern int saved_stdout; 
extern int saved_stdin;
extern int saved_stderr;


char *sh_read_line();
char **sh_split_line(char *line);
char **sh_extract_commands(char *line);
int foreground(char **args);
void add_to_list_bg(bg_process *new);
int background(char **args);
void remove_bg_list(bg_process *temp);
void check_bg_if_ended();
void sh_exec(char **args,char* line_execute_pastevnts);

void redirect(char *input);
void redirect_io(char *input_file, char *output_file, int append);


int fg(char *argv[]);
int bg(char *argv[]);

void activities();

int ping(char** args);
void kill_bg();
void ignore_promptprint();

void  SIGINT_handler(int); 
void  SIGTSTP_handler(int sig);

int pipes(char* input_line);
char **split_line(char *line);
char **extract_commands(char *line);
void execute_why(char **argv);

void spec11(char *input_line);


#endif
// 