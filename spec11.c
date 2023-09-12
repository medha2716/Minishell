#include "headers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void spec11(char *input_line)
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
        return ;
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
            return;
        }
    }
    char **commands_separated_by_pipe = extract_commands(input_line);

    int i = 0;

    for (; i < programno; i++)
    {

        char **args = split_line(commands_separated_by_pipe[i]); // specification 2
        int flag_out = 0;
        // Check for redirection operators

        char *input_file = NULL;
        char *output_file = NULL;
        int append = 0;

        for (int i = 0; args[i] != NULL; i++)
        {

            if (strcmp(args[i], ">") == 0) // there may be no i+1 then give error
            {
                if (flag_out)
                {
                    printf(MAG);
                    printf("error: invalid input output redirection\n");
                    printf(COL_RESET);
                    return;
                }
                append = 0;
                output_file = args[i + 1];
                flag_out = 1;
            }
            else if (strcmp(args[i], ">>") == 0)
            {
                if (flag_out)
                {
                    printf(MAG);
                    printf("error: invalid input output redirection\n");
                    printf(COL_RESET);
                    return;
                }
                append = 1;
                output_file = args[i + 1];
                flag_out = 1;
            }

            if (strcmp(args[i], "<") == 0)
            {
                input_file = args[i + 1];
            }
        }
        if (!flag_out)
        {
            output_file = NULL;
            append = 0;
        }

        if (args[0] != NULL) // no command
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
                redirect_io(input_file, output_file, append);
                char **argv = (char **)malloc(5000 * sizeof(char *));
                for (int i = 0; args[i] != NULL; i++)
                {
                    if ((strcmp(args[i], ">>") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], "<") == 0))
                        break;
                    argv[i] = args[i];
                }

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
        free(args);
    }

    // free memory before next command

    // Close any remaining pipe ends in the parent
    for (int j = 0; j < programno - 1; j++)
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

    wait(NULL);

    return;
}
