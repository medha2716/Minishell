#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

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
    }
    else if (strcmp("pastevents", argv[0]) == 0)
    {
        read_command();
    }
    else if (strcmp("seek", argv[0]) == 0)
    {
        seek(argv);
    }
    else if (strcmp("fg", argv[0]) == 0)
    {
        fg(argv);
    }
    else if (strcmp("bg", argv[0]) == 0)
    {
        bg(argv);
    }
    else if (strcmp("activities", argv[0]) == 0)
    {
        activities();
    }
    else if (strcmp("ping", argv[0]) == 0)
    {
        ping(argv);
    }
    else
    {

        execvp(argv[0], argv);
        // sh_exec(args,input);
        perror(MAG);
        perror("execvp");
        perror(COL_RESET);
    }
}

int pipes(char *input_line)
{
    saved_stdout = dup(1);
    saved_stdin = dup(0);
    int status;

    size_t input_size = strlen(input_line);
    int programno = 1;

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
    wait(NULL);
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

wait(NULL);

return 0;
}









// }
// else if (strcmp("warp", argv[0]) == 0)
// {

//     // if return with error dont add to history
//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         warp(argv);
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }
// }
// else if (strcmp("proclore", argv[0]) == 0)
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         if (!argv[1])
//             proclore("self");
//         else
//         {
//             proclore(argv[1]);
//         }
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }
// }
// else if ((strcmp("pastevents", argv[0]) == 0) && (argv[1]))
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         if (strcmp("purge", argv[1]) == 0)
//             pastevents_purge();
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }
// }
// else if (strcmp("pastevents", argv[0]) == 0)
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         read_command();
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }
// }
// else if (strcmp("seek", argv[0]) == 0)
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         seek(argv);
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }

// }
// // else if (strcmp("fg", argv[0]) == 0)
// // {
// //     fg(argv);
// // }
// // else if (strcmp("bg", argv[0]) == 0)
// // {
// //     bg(argv);
// // }
// else if (strcmp("activities", argv[0]) == 0)
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         activities();
//         exit(1);
//     }
//     else
//     {
//         if (i > 0)
//             close(pipe_fd[i - 1][0]);
//         if (i < programno - 1)
//             close(pipe_fd[i][1]);
//     }
// }
// // else if (strcmp("ping", argv[0]) == 0)
// // {
// //     ping(argv);
// // }
// else
// {

//     int pid = fork();
//     foreground_running_pid = pid;
//     if (pid < 0)
//     {
//         perror(MAG);
//         perror("fork");
//         perror(COL_RESET);
//     }
//     else if (pid == 0)
//     {
//         if (i > 0)
//         {
//             close(pipe_fd[i - 1][1]);
//             dup2(pipe_fd[i - 1][0], 0);
//             close(pipe_fd[i - 1][0]);
//         }
//         if (i < programno - 1)
//         {
//              close(pipe_fd[i][0]);
//             dup2(pipe_fd[i][1], 1);
//             close(pipe_fd[i][1]);
//         }
//         for (int j = 0; j < programno - 1; j++)
//         {
//             close(pipe_fd[j][0]);
//             close(pipe_fd[j][1]);
//         }
//         execvp(argv[0], argv);
//         // sh_exec(args,input);
//         perror(MAG);
//         perror("execvp");
//         perror(COL_RESET);
//         exit(1);
// }
//  else
//             {
//                 if (i > 0)
//                     close(pipe_fd[i - 1][0]);
//                 if (i < programno - 1)
//                     close(pipe_fd[i][1]);
//             }