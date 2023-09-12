#include "headers.h"

char *OLDPWD;
int no_of_bg;
bg_process *Head_bg;
char HOME[1024];
long time_flag;
char arg_0[1024];
int foreground_running_pid;
int saved_stdout;
int saved_stdin;
int saved_stderr;

char *sh_read_line()
{

    char *line = NULL;
    size_t buflen = 0; // The buflen variable is used to store the size of the buffer allocated by the getline function to hold the input line. When you pass a pointer to buflen to the getline function, the function will update the value of buflen to reflect the actual size of the allocated buffer.

    ssize_t chars_read = getline(&line, &buflen, stdin);

    if (chars_read == -1)
    {

        if (feof(stdin))
        {
            // Ctrl+D (EOF) detected
            free(line);
            printf(PNK);
            printf("\nCtrl+D detected. Logging out of shell..\n");
            printf(COL_RESET);
            kill_bg();
            exit(0);
        }
        else
        {
            // An error occurred while reading input
            printf(MAG);
            printf("An error occurred while reading input.\n");
            printf(COL_RESET);
        }
    }

    return line;
}

char **sh_split_line(char *line)
{
    int no_of_tokens = 0;
    int tokens_array_size = 20;                                 // initial assumption for no of tokens
    char **tokens = malloc(tokens_array_size * sizeof(char *)); // array of tokens

    char *delimiters = " \t\r\n";
    char *token = strtok(line, delimiters);

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
    return tokens;
}

char **sh_extract_commands(char *line) // tokens in this are entire commands not particular words
{
    int no_of_tokens = 0;
    int tokens_array_size = 20;                                 // initial assumption for no of tokens
    char **tokens = malloc(tokens_array_size * sizeof(char *)); // array of tokens
    char *stor_line = (char *)malloc(strlen(line) + 1);
    strcpy(stor_line, line);
    char *delimiters = ";&";
    char *token = strtok(line, delimiters);
    int pos = 0;

    while (token != NULL)
    {
        pos += strlen(token) + 1;
        char *str = (char *)calloc(strlen(stor_line) + 1, sizeof(char));
        //    printf("%s%d ",token,pos);
        if (pos < strlen(stor_line))
        {
            if (stor_line[pos - 1] == '&')
            {
                strcat(str, token);
                strcat(str, " &");
                tokens[no_of_tokens] = str;
            }
            else
            {
                tokens[no_of_tokens] = token;
            }
        }
        else
        {
            tokens[no_of_tokens] = token;
        }

        no_of_tokens++;

        if (no_of_tokens >= tokens_array_size)
        {
            tokens_array_size = (int)(tokens_array_size * 2);
            tokens = realloc(tokens, tokens_array_size * sizeof(char *));
        }

        token = strtok(NULL, delimiters);
    }

    tokens[no_of_tokens] = NULL;
    free(stor_line);
    return tokens;
}

int foreground(char **args)
{

    pid_t pid = fork();

    // struct sigaction sa;
    // sa.sa_handler = SIGINT_handler;

    foreground_running_pid = pid;
    if (pid < 0)
    {
        perror(MAG);
        perror("Couldn't make a foreground fork of process!");
        perror(COL_RESET);
    }

    if (pid == 0)
    {

        if (execvp(args[0], args))
        {
            printf(MAG);
            printf("ERROR: '%s' is not a valid command\n", args[0]);
            printf(COL_RESET);
            return 1; // what if arguments are the problem?
        }
    }
    else
    {
        // sigaction(SIGINT, &sa, NULL);
        int status;
        waitpid(pid, &status, WUNTRACED); // waits till process stops or terminates
        // printf("hi\n");
    }

    return 0;
}

void SIGINT_handler(int sig)
{

    if ((foreground_running_pid > 0) && (kill(foreground_running_pid, 0) == 0))
    {
        printf(PNK);
        printf("Received Ctrl+C. Sending SIGINT to the foreground process...\n");
        printf(COL_RESET);
        kill(foreground_running_pid, SIGINT);
        foreground_running_pid = -1;
    }
    else if (foreground_running_pid < 0)
    {
        signal(sig, SIG_IGN);
        ignore_promptprint();
    }
}
void SIGTSTP_handler(int sig)
{

    if ((foreground_running_pid > 0) && (kill(foreground_running_pid, 0) == 0))
    {
        printf(PNK);
        printf("Received Ctrl+Z. Stopping the foreground process and pushing it to the background...\n");
        printf(COL_RESET);
        kill(foreground_running_pid, SIGTSTP);

        //   if (setpgid(0, 0) < 0) {
        //     perror("setpgid");
        //     exit(EXIT_FAILURE);
        // }

        // Create a new session and detach from the terminal
        // if (setsid() < 0) {
        //     perror("setsid failed");
        //     exit(EXIT_FAILURE);
        // }

        // Attempt to send a SIGHUP signal to the process to detach it from the terminal
        // if (kill(foreground_running_pid, SIGHUP) < 0)
        // {
        //     perror("kill failed");
        //     exit(EXIT_FAILURE);
        // }

        ++no_of_bg;
        bg_process *new_bg_process = (bg_process *)malloc(sizeof(bg_process));
        new_bg_process->command = (char *)malloc(sizeof(char) * 5000);
        strcpy(new_bg_process->command, arg_0);
        new_bg_process->next = NULL;
        new_bg_process->prev = NULL;
        new_bg_process->pid = foreground_running_pid;
        new_bg_process->status = -100;
        add_to_list_bg(new_bg_process);
        printf("[%d] %d\n", no_of_bg, foreground_running_pid);

        foreground_running_pid = -1;
    }
    else if (foreground_running_pid < 0)
    {
        signal(sig, SIG_IGN);
        ignore_promptprint();
    }
}

void add_to_list_bg(bg_process *new)
{
    new->prev = Head_bg;
    new->next = Head_bg->next;
    if (Head_bg->next)
        Head_bg->next->prev = new;
    Head_bg->next = new;
    return;
}

int background(char **args)
{

    pid_t pid = fork();

    if (pid < 0)
    {
        perror(MAG);
        perror("Couldn't make a background fork of process!");
        perror(COL_RESET);
    }

    if (pid == 0)
    {

        // setpgid(0, 0);//detach child process from parent process
        // Disable terminal job control for the child
        if (setpgid(0, 0) < 0)
        {
            perror("setpgid");
            exit(EXIT_FAILURE);
        }

        // kill(getppid(),SIGCHLD);//sends signal to parent process getppid() used to get parent process id
        if (execvp(args[0], args))
        {
            printf(MAG);
            printf("ERROR: '%s' is not a valid command\n", args[0]);
            printf(COL_RESET);
            return 1; // what if arguments are the problem?
        }
    }
    else
    {

        ++no_of_bg;
        bg_process *new_bg_process = (bg_process *)malloc(sizeof(bg_process));
        new_bg_process->command = (char *)malloc(sizeof(char) * 5000);
        strcpy(new_bg_process->command, args[0]);
        new_bg_process->next = NULL;
        new_bg_process->prev = NULL;
        new_bg_process->pid = pid;
        // int status;
        // waitpid(pid, &status, 0);
        new_bg_process->status = -100;
        add_to_list_bg(new_bg_process);
        printf("[%d] %d\n", no_of_bg, pid);
    }
    return 0;
}

void remove_bg_list(bg_process *temp)
{

    bg_process *next = temp->next;
    bg_process *prev = temp->prev;
    if (next != NULL)
        next->prev = prev;
    if (prev != NULL)
        prev->next = next;

    return;
}

void check_bg_if_ended()
{

    bg_process *temp = Head_bg->next;

    while (temp != NULL)
    {
        // printf("hi\n");
        // printf("bitch why %d\n",temp->pid);

        int status;

        int result = waitpid(temp->pid, &status, WNOHANG);
        // printf("%d\n",temp->pid);
        // printf("%d\n",status);
        if (result == 0)
        {
            // printf("Process with PID %d is still running.\n", temp->pid);
            // empty cause i dont want to delete this "if" i will forget the logic
        }
        else if (result == temp->pid)
        {

            if (WIFEXITED(status))
            {
                printf("%s exited normally (%d)\n", temp->command, temp->pid);
                remove_bg_list(temp);
                no_of_bg--;
            }
            else if (WIFSTOPPED(status))
            {
                printf("%s stopped (%d)\n", temp->command, temp->pid);
            }
            else if (WIFSIGNALED(status))
            {
                printf("%s ended abnormally (%d) due to signal: %d\n", temp->command, temp->pid, WTERMSIG(status));
                remove_bg_list(temp);
                no_of_bg--;
            }
        }

        temp = temp->next;
    }
}

void sh_exec(char **args, char *line_execute_pastevnts)
{
    int i = 0;

    // check for pastevents separately
    if (strcmp("warp", args[i]) == 0)
    {

        warp(args); // if return with error dont add to history
        // printf("%d",i);
        return;
    }
    else if (strcmp("peek", args[0]) == 0)
    {

        peek(args);
        return;
    }
    else if(strcmp("neonate", args[0]) == 0)
    {
        if(args[1]==NULL)
            {
                printf("ERROR\n");
                return;
            }
             if(args[2]==NULL)
            {
                printf("ERROR\n");
                return;
            }
        neonate(atoi(args[2])); //if not a number give error
        return;
    }
    else if (strcmp("activities", args[0]) == 0)
    {

        activities();
        return;
    }
    else if (strcmp("fg", args[0]) == 0)
    {
        fg(args);
        return;
    }
    else if (strcmp("bg", args[0]) == 0)
    {
        fg(args);
        return;
    }
    else if (strcmp("ping", args[0]) == 0)
    {
        ping(args);
        return;
    }
    else if ((strcmp("pastevents", args[0]) == 0) && (args[1]))
    {
        if (strcmp("purge", args[1]) == 0)
            pastevents_purge();
        else if (strcmp("execute", args[1]) == 0)
        {
            if (!args[2])
            {
                printf(MAG);
                printf("ERROR: pastevents execute: too few arguments\n");
                printf(COL_RESET);
            }
            else
            {
                int pos = atoi(args[2]);
                if (pos < 1)
                {
                    printf(MAG);
                    printf("ERROR: pastevents: No command at entered position!\n");
                    printf(COL_RESET);
                    return;
                }
                // printf("%d\n",pos);
                execute_command(pos, line_execute_pastevnts);
            }
        }
    }
    else if (strcmp("pastevents", args[0]) == 0)
    {
        read_command();
    }
    else if (strcmp("seek", args[0]) == 0)
    {
        seek(args);
    }
    else if (strcmp("proclore", args[0]) == 0)
    {
        if (!args[1])
            proclore("self");
        else
        {
            proclore(args[1]);
        }
    }
    else
    {

        int i = 0;
        while (args[i] != 0)
        {
            i++;
        }
        if (strcmp(args[i - 1], "&") == 0)
        {

            args[i - 1] = NULL;
            // printf("%s %s\n",args[0],args[1]);
            char **args_bg = (char **)calloc(5000, sizeof(char *));
            for (int j = 0; j < (i - 1); j++)
            {
                args_bg[j] = (char *)calloc(5000, sizeof(char));
                strcpy(args_bg[j], args[j]);
            }
            args_bg[i - 1] = NULL;

            background(args_bg);
            // printf("execute %s %s as background process\n", args[0], args[1]);
            for (int j = 0; j < (i - 1); j++)
            {
                free(args_bg[j]);
            }
            free(args_bg);
        }
        else
        {
            foreground(args);
            // printf("execute  %s %s normally\n", args[0], args[1]);
        }

        return;
    }
}

int main()
{
    saved_stdout = dup(1);
    saved_stdin = dup(0);
    saved_stderr = dup(STDERR_FILENO);
    no_of_bg = 0;
    OLDPWD = calloc(PATH_MAX, sizeof(char));
    Head_bg = (bg_process *)malloc(sizeof(bg_process));
    Head_bg->next = NULL;
    Head_bg->prev = NULL;
    Head_bg->command = "START_DUMMY";

    getcwd(HOME, sizeof(HOME));

    // shell is a loop that accepts commands

    while (1)
    {
        prompt(); // specification 1
        fflush(stdout);

        char *line = sh_read_line(); // accept command from user

        foreground_running_pid = -1;

        if (signal(SIGINT, SIGINT_handler) == SIG_ERR)
        {
            printf("SIGINT install error\n");
            exit(1);
        }
        if (signal(SIGTSTP, SIGTSTP_handler) == SIG_ERR)
        {
            printf("SIGSTP install error\n");
            exit(1);
        }
        check_bg_if_ended();

        long start_of_process = time(NULL);

        // PASTEVENTS HISTORY!!!!!!!!!
        char line_copy[5000];
        strcpy(line_copy, line);

        line_copy[strlen(line_copy) - 1] = '\0';

        int flag_add_to_history = 0;
        flag_add_to_history = check_if_pastevents(line_copy);
        if (flag_add_to_history)
            add_command(line_copy);
        strcpy(arg_0, line_copy);

        char **commands_separated_by_semicolon = sh_extract_commands(line);

        int i = 0;

        while (commands_separated_by_semicolon[i] != NULL)
        {

            if ((strstr(commands_separated_by_semicolon[i], "|") == NULL))
            {


                if ((strstr(commands_separated_by_semicolon[i], ">") != NULL) || (strstr(commands_separated_by_semicolon[i], ">>") != NULL) || (strstr(commands_separated_by_semicolon[i], "<") != NULL))
                {
                    // char **tokens = sh_split_line(commands_separated_by_semicolon[i]);
                    redirect(commands_separated_by_semicolon[i]);
                }

                else
                {
                    char **tokens = sh_split_line(commands_separated_by_semicolon[i]);
                    
                    if (tokens[0] != NULL) // no command
                    {
                        sh_exec(tokens, line_copy);
                    }

                    free(tokens); // free memory before next command
                }
                i++;
            }
            else if ((strstr(commands_separated_by_semicolon[i], ">") == NULL) && (strstr(commands_separated_by_semicolon[i], ">>") == NULL) && (strstr(commands_separated_by_semicolon[i], "<") == NULL))
            {
                pipes(commands_separated_by_semicolon[i]);
                i++; // this is a very important statement; please make sure you never make this mistake :)
            }
            else
            {
                spec11(commands_separated_by_semicolon[i]);
                i++;
            }
        }
        long end_of_process = time(NULL);
        time_flag = end_of_process - start_of_process;
        char *result = strstr(line_copy, "pastevents execute");
        if (result != NULL)
        {
            char *changed_line = add_to_history(line_copy);
            add_command(changed_line);
        }
        free(commands_separated_by_semicolon);
        free(line);
    }
    // free(HOME);
}