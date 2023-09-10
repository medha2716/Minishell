#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_PROGRAMS 10 // Maximum number of programs in the pipeline
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
            tokens[no_of_tokens][c++]=' ';
            for(int i=0;i<strlen(token);i++)
                if(token[i]!='\'')
                    tokens[no_of_tokens][c++]=token[i];
        }
        else if (token[0] == '\'')
        {
            inQuotes = 1;
            c = 0;
            tokens[no_of_tokens]=malloc(sizeof(char)*1024);
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
int main()
{
    int status;
    int saved_stdout = dup(1);
     int saved_stdin = dup(0);
 int saved_stderr = dup(STDERR_FILENO);
    int pipe_fd[MAX_PROGRAMS - 1][2]; // Array of pipes for communication
    pid_t pid[MAX_PROGRAMS];          // Array to store process IDs
    char *input_line = NULL;
    size_t input_size = 0;
    int programno = 0;
    printf("Enter a command pipeline (e.g., 'cmd1 | cmd2 | cmd3'): ");
    getline(&input_line, &input_size, stdin);
    for (int j = 0; j < strlen(input_line); j++)
    {
        if (input_line[j] == '|')
            programno++;
    }
    // Tokenize the input based on '|'
    char **commands_separated_by_pipe = extract_commands(input_line);

    int i = 0;
    
    while (commands_separated_by_pipe[i] != NULL)
    {

        char **argv = split_line(commands_separated_by_pipe[i]); // specification 2

        if (argv[0] != NULL) // no command
        {
            // printf("%s ",argv[0]);

            // Create a pipe for communication between processes
            if (pipe(pipe_fd[i]) < 0)
            {
                perror("Pipe failed");
                exit(1);
            }

            // Create a new process
            pid[i] = fork();

            if (pid[i] < 0)
            {
                perror("Fork failed");
                exit(1);
            }

            if (pid[i] == 0)
            {
                // Child process

                // for(int j=0;j<i;j++)
                // {
                //     if(j!=i-1)
                //         close(pipe_fd[j][0]);
                //     if(j!=i)
                //         close(pipe_fd[j][1]);
                // }
                if(i==0)
                {
                    dup2(saved_stdin, 1);
                    close(saved_stdin);
                }
                if (i > 0)
                {
                    // Redirect stdin to the previous pipe
                    close(pipe_fd[i - 1][1]);
                    dup2(pipe_fd[i - 1][0], 0);
                    close(pipe_fd[i - 1][0]);
                }

                if (i < programno)
                {

                    // Redirect stdout to the next pipe
                    close(pipe_fd[i][0]);
                    dup2(pipe_fd[i][1], 1);
                    close(pipe_fd[i][1]);
                    
                }

                // Execute the program
                if (i == programno)
                {
                    dup2(saved_stdout, 1);
                    close(saved_stdout);
                }
                redirect(argv);
                
                // perror("Exec failed");
                // exit(1);
            }
            else
            {

                // Parent process
                if (i > 0)
                {
                    // Close the previous pipe ends in the parent
                    close(pipe_fd[i - 1][0]);
                    close(pipe_fd[i - 1][1]);
                }
            }
            i++;
        }

        free(argv);

    } // free memory before next command

    // Close any remaining pipe ends in the parent
    for (int j = 0; j < i - 1; j++)
    {
        close(pipe_fd[j][0]);
        close(pipe_fd[j][1]);
    }

    // // Wait for all child processes to complete
    // for (int j = 0; j < i; j++)
    // {
    //     waitpid(pid[j], &status, 0);
    // }

    // Free dynamically allocated memory
    free(input_line);

    return 0;
}
