#include "headers.h"

char *OLDPWD;
int no_of_bg ;
bg_process *Head_bg;
char HOME[1024];


char *sh_read_line()
{

    char *line = NULL;
    size_t buflen = 0; // The buflen variable is used to store the size of the buffer allocated by the getline function to hold the input line. When you pass a pointer to buflen to the getline function, the function will update the value of buflen to reflect the actual size of the allocated buffer.

    ssize_t chars_read = getline(&line, &buflen, stdin);

    if (chars_read == -1)
    {
        printf("An error occurred while reading input.\n");
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
        char *str = (char *)malloc(strlen(stor_line) + 1);
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
void sigchld_handler(int signo)
{

    // update status

    int pid, status;
    // int serrno;
    //   serrno = errno;
    while (1)
    {
        pid = waitpid(-1, &status, WNOHANG);
        if (pid < 0)
        {
            // perror("waitpid");
            break;
        }
        if (pid == 0)
            break;
        bg_process *temp = Head_bg->next;
        while (temp != NULL)
        {
            if (temp->pid == pid)
                temp->status = status;
            temp = temp->next;
        }
    }
    //   errno = serrno;
}

int foreground(char **args)
{

    long start_of_process = time(NULL);
    
    pid_t pid = fork();
    if (pid < 0)
        perror("Couldn't make a foreground fork of process!");

    if (pid == 0)
    {
        if (execvp(args[0], args))
        {
            printf("ERROR: '%s' is not a valid command\n", args[0]);

            return 1; // what if arguments are the problem?
        }
    }
    else
    {

        // Pause bg processes
        // bg_process* temp=Head_bg->next;
        // while(temp!=NULL)
        // {
        //     if((temp->status)==(-100))
        //         kill(temp->pid, SIGSTOP);
        // }
        // Wait for fg process to finish
        // wait(NULL);

        wait(NULL);

        //Resume bg processes
        // bg_process* temp=Head_bg->next;
        // while(temp!=NULL)
        // {
        //     if((temp->status)==28) //28 is for SIGSTOP //also put code for if it ends due to a signal other than 28
        //         {
        //             kill(temp->pid, SIGCONT);
        //             temp->status=-100;
        //         }
        // }

        // ASKKKKKK
    }
      int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
    {
        // Child process succeeded
        long end_of_process = time(NULL);
        long time = end_of_process - start_of_process;

        if (time >= 2)
        {
            printf("# %ss for %ld seconds\n", args[0], time);
        }
    }

    return 0;
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
    signal(SIGCHLD, sigchld_handler);
    pid_t pid = fork();

    if (pid < 0)
        perror("Couldn't make a background fork of process!");

    if (pid == 0)
    {

        // setpgid(0, 0);//detach child process from parent process

        if (execvp(args[0], args))
        {
            printf("ERROR: '%s' is not a valid command\n", args[0]);
            return 1; // what if arguments are the problem?
        }
    }
    else
    {
        ++no_of_bg;
        bg_process *new_bg_process = (bg_process *)malloc(sizeof(bg_process));
        new_bg_process->command = (char *)malloc(sizeof(char) * 1024);
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
    if (temp->prev)
        temp->prev->next = temp->next;
    free(temp);
    return;
}

void check_bg_if_ended()
{

    bg_process *temp = Head_bg->next;
    while (temp != NULL)
    {
        int status = temp->status;

        // if (result == 0)
        // {
        //     // printf("Process with PID %d is still running.\n", temp->pid);
        //     //empty cause i dont want to delete this "if" i will forget the logic
        // }
        // else if (result == temp->pid)
        // {
        if (WIFEXITED(status))
        {
            printf("%s exited normally (%d)\n", temp->command, temp->pid);
            remove_bg_list(temp);
            no_of_bg--;
        }
        else if (WIFSIGNALED(status))
        {
            printf("%s ended abnormally (%d) due to signal: %d\n", temp->command, temp->pid, WTERMSIG(status));
            remove_bg_list(temp);
            no_of_bg--;
        }

        // }
        // else if (result == -1)
        // {
        //     perror("waitpid");
        //     // Handle error if necessary
        // }
        temp = temp->next;
    }
}

void sh_exec(char **args, char *line_execute_pastevnts)
{
    int i = 0;
    char **args_bg = (char **)malloc(100 * sizeof(char *));
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
    else if ((strcmp("pastevents", args[0]) == 0) && (args[1]))
    {
        if (strcmp("purge", args[1]) == 0)
            pastevents_purge();
        else if (strcmp("execute", args[1]) == 0)
        {
            if (!args[2])
            {
                printf("ERROR: pastevents execute: too few arguments\n");
            }
            else
            {
                int pos = atoi(args[2]);
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
    else
    {

        int i = 0;
        while (args[i] != 0)
        {
            i++;
        }
        if (strcmp(args[i - 1], "&") == 0)
        {
            // args[i-1]=NULL;
            // printf("%s %s\n",args[0],args[1]);
            for (int j = 0; j < (i - 1); j++)
            {
                args_bg[j] = (char *)malloc(sizeof(char) * 1024);
                strcpy(args_bg[j], args[j]);
            }
            args_bg[i - 1] = NULL;
            background(args_bg);
            // printf("execute %s %s as background process\n", args[0], args[1]);
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
    no_of_bg = 0;
    OLDPWD = calloc(PATHno_of_tokens_MAX, sizeof(char));
    Head_bg = (bg_process *)malloc(sizeof(bg_process));
    Head_bg->next = NULL;
    Head_bg->prev = NULL;
    Head_bg->command = "START_DUMMY";

    getcwd(HOME, sizeof(HOME));
    // printf("%s\n",HOME);
    // shell is a loop that accepts commands
    while (1)
    {
        prompt();                    // specification 1
        char *line = sh_read_line(); // accept command from user

        check_bg_if_ended();
        // PASTEVENTS HISTORY!!!!!!!!!
        char line_copy[5000];
        strcpy(line_copy, line);
        line_copy[strlen(line_copy) - 1] = '\0';
        int flag_add_to_history = 0;
        flag_add_to_history = check_if_pastevents(line_copy);
        if (flag_add_to_history)
            add_command(line_copy);

        char **commands_separated_by_semicolon = sh_extract_commands(line);

        int i = 0;
        while (commands_separated_by_semicolon[i] != NULL)
        {
            char **tokens = sh_split_line(commands_separated_by_semicolon[i]); // specification 2
            i++;
            // printf("%s",commands_separated_by_semicolon[i]);
            // printf("%s",tokens[0]);
            if (tokens[0] != NULL) // no command
            {
                
                sh_exec(tokens, line_copy);
            }

            free(tokens);
        } // free memory before next command

        free(commands_separated_by_semicolon);
        free(line);
    }
    // free(HOME);
}
