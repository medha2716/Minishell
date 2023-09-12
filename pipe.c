#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int check_pipe(const char *string) {
    int len = strlen(string);
    
    // check for a pipe at the beginning
    if (string[0] == '|')
        return 0;
    
    // check for a pipe at the end
    if (string[len - 1] == '|')
        return 0;

    for (int i = 1; i < len - 1; i++) {
        if (string[i] == '|') {
            // check left of the pipe
            int left = i - 1;
            while (left >= 0 && (string[left] == ' ' || string[left] == '\t' || string[left] == '\n'))
                left--;
            
            if (left < 0 || string[left] == '|')
            {
                 return 0;
            }
               

            // check right of the pipe
            int right = i + 1;
            while (right < len && (string[right] == ' ' || string[right] == '\t' || string[right] == '\n'))
                right++;
            if (right >= len || string[right] == '|')
                return 0;
        }
    }

    return 1;
}

char **split_line(char *line)
{

    char *token;
    int no_of_tokens = 0;
    int tokens_array_size = 20;                                 // initial assumption for the number of tokens
    char **tokens = malloc(tokens_array_size * sizeof(char *)); // array of tokens

    char *delimiter = " \t\r\n";
    int inQuotes = 0;
    char *start = line;
    int c;
    while ((token = strtok_r(start, delimiter, &start)))
    {
        if (inQuotes)
        {
            tokens[no_of_tokens][c++] = ' ';
            for (int i = 0; i < strlen(token); i++)
                if (token[i] != '\'')
                    tokens[no_of_tokens][c++] = token[i];
        }
        else if (token[0] == '\'')
        {
            inQuotes = 1;
            c = 0;
            tokens[no_of_tokens] = malloc(sizeof(char) * 1024);
            for (int i = 1; i < strlen(token); i++)

            {
                tokens[no_of_tokens][c] = token[i];
                c++;
            }
        }
        else
        {
            tokens[no_of_tokens++] = token;
        }

        if (token[strlen(token) - 1] == '\'')
        {
            inQuotes = 0;
            tokens[no_of_tokens++][c] = '\0';
        }

        if (no_of_tokens >= tokens_array_size)
        {
            tokens_array_size = (int)(tokens_array_size * 2);
            tokens = realloc(tokens, tokens_array_size * sizeof(char *));
        }
    }
    tokens[no_of_tokens] = NULL;
    // for (int i = 0; i < 5; i++)
    // {
    //     printf("%s\n", tokens[i]);
    // }
    return tokens;
}
char **extract_commands(char *line) // tokens in this are entire commands not particular words
{
    int no_of_tokens = 0;
    int tokens_array_size = 20;                                 // initial assumption for no of tokens
    char **tokens = malloc(tokens_array_size * sizeof(char *)); // array of tokens

    char *delimiters = "|";
    char *token = strtok(line, delimiters);
    int pos = 0;

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

void execute_why(char **argv)
{
     if (strcmp("peek", argv[0]) == 0)
        {
            peek(argv);
            
        }
        else if (strcmp("warp", argv[0]) == 0)
        {

            warp(argv); // if return with error dont add to history
       
        }
        else if (strcmp("proclore", argv[0]) == 0)
        {
            if (!argv[1])
                proclore("self");
            else
            {
                proclore(argv[1]);
            }
          
        }
        else if ((strcmp("pastevents", argv[0]) == 0) && (argv[1]))
        {
            if (strcmp("purge", argv[1]) == 0)
                pastevents_purge();
            else if (strcmp("execute", argv[1]) == 0)
            {
                if (!argv[2])
                {
                    printf(MAG);
                    printf("ERROR: pastevents execute: too few arguments\n");
                    printf(COL_RESET);
                    return;
                }
                else
                {
                    int pos = atoi(argv[2]);
                    if (pos < 1)
                    {
                        printf(MAG);
                        printf("ERROR: pastevents: No command at entered position!\n");
                        printf(COL_RESET);
                        return;
                    }
                    // printf("%d\n",pos);
                    redirect_execute_command(pos);
                }
            }
            
        }
        else if (strcmp("pastevents", argv[0]) == 0)
        {
            read_command();
            
        }
        else if (strcmp("seek", argv[0]) == 0)
        {
            seek(argv);
            
        }
        else if (strcmp("neonate", argv[0]) == 0)
        {
            if (argv[1] == NULL)
            {
                printf(MAG);
                printf("neonate: usage: neonate -n [time_arg]\n");
                printf(COL_RESET);
                return;
            }
            if (argv[2] == NULL)
            {
                printf(MAG);
                printf("neonate: usage: neonate -n [time_arg]\n");
                printf(COL_RESET);
                return;
            }
            neonate(atoi(argv[2])); // if not a number give error
            
        }
        else if (strcmp("iMan", argv[0]) == 0)
        {
            if (argv[1] == NULL)
            {
                printf(MAG);
                printf("iMan: usage: iMan <command_name>\n");
                printf(COL_RESET);
                return;
            }

            iman(argv[1]); // if not a number give error
          
        }
        else if (strcmp("activities", argv[0]) == 0)
        {

            activities();
            
        }
        else if (strcmp("fg", argv[0]) == 0)
        {
            if (argv[1] == NULL)
            {
                printf(MAG);
                printf("fg: usage: fg <pid>\n");
                printf(COL_RESET);
                return;
            }
            fg(argv);
          
        }
        else if (strcmp("bg", argv[0]) == 0)
        {
            if (argv[1] == NULL)
            {
                printf(MAG);
                printf("bg: usage: bg <pid>\n");
                printf(COL_RESET);
                return;
            }
            bg(argv);
     
        }
        else if (strcmp("ping", argv[0]) == 0)
        {
            if (argv[1] == NULL)
            {
                printf(MAG);
                printf("ping: usage: ping <pid> <signal_number>\n");
                printf(COL_RESET);
                return;
            }
            if (argv[2] == NULL)
            {
                printf(MAG);
                printf("ping: usage: ping <pid> <signal_number>\n");
                printf(COL_RESET);
                return;
            }
            ping(argv);
    
        }
        else
        {
            if (execvp(argv[0], argv))
            {
                printf(MAG);
                printf("ERROR: '%s' is not a valid command\n", argv[0]);
                printf(COL_RESET);
                return;
            }
 
        }
}

int pipes(char *input_line)
{
    saved_stdout = dup(1);
    saved_stdin = dup(0);
    int status;

    size_t input_size = strlen(input_line);
    int programno = 1;

    if(!check_pipe(input_line))
    {
        printf(MAG);
        printf("Invalid use of pipe\n");
        printf(COL_RESET);
        return 1;
    }

    for (int j = 0; j < strlen(input_line); j++)
    {
        if (input_line[j] == '|')
            programno++; 
    }

    int pipe_fd[programno - 1][2]; // Array of pipes for communication
    pid_t pid[programno];          // Array to store process IDs
    // Tokenize the input based on '|'

    for (int i = 0; i < programno; i++)
    {
        if (pipe(pipe_fd[i]) < 0)
        {
            perror(MAG);
            perror("Pipe failed");
            perror(COL_RESET);
            return 1;
        }
    }
    char **commands_separated_by_pipe = extract_commands(input_line);
  
    int i = 0;

    for (; i < programno; i++)
    {
        
        
        char **argv = split_line(commands_separated_by_pipe[i]); // specification 2

        if (argv[0] != NULL) // no command
        {
            // printf("%s\n",argv[0]);
            // if (strcmp("peek", argv[0]) == 0)
            // {
                
            int pid = fork();
            foreground_running_pid = pid;
            if (pid < 0)
            {
                perror(MAG);
                perror("fork");
                perror(COL_RESET);
            }
            else if (pid == 0)
            {
                
                if (i > 0)
                {
                    // close(pipe_fd[i - 1][1]);
                    dup2(pipe_fd[i - 1][0], 0);
                    close(pipe_fd[i - 1][0]);
                }
                
                if (i < programno - 1)
                {
                    // close(pipe_fd[i][0]);
                    dup2(pipe_fd[i][1], 1); 
                    close(pipe_fd[i][1]);    
                }
                for (int j = 0; j <= programno - 1; j++)
                {
                    close(pipe_fd[j][0]);
                    close(pipe_fd[j][1]);
                }
                
                
                execute_why(argv);
                exit(1);
            }
            else
            {
                if (i > 0)
                    close(pipe_fd[i - 1][0]);
                if (i < programno - 1)
                    close(pipe_fd[i][1]);
                // wait(NULL);
                // dup2(saved_stdin, 0);
                // close(saved_stdin);
                // dup2(saved_stdout, 1);
                // close(saved_stdout);
            }

           
        }
        free(argv);
    }

 // free memory before next command

// Close any remaining pipe ends in the parent
for (int j = 0; j < programno-1; j++)
{
    close(pipe_fd[j][0]);
    close(pipe_fd[j][1]);
}

// Wait for all child processes to complete
for (int i = 0; i < programno; i++)
{
    int status;
    waitpid(pid[i], &status, WUNTRACED);
}

// Free dynamically allocated memory
// for (int j = 0; j < i; j++)
// {
//     close(pipe_fd[j][0]);
//     close(pipe_fd[j][1]);
// }

dup2(saved_stdin, 0);
close(saved_stdin);
dup2(saved_stdout, 1);
close(saved_stdout);

wait(NULL); // wait for this function to end otherwise prompt gets printed first

return 0;
}



