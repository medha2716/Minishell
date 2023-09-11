// #include "headers.h"

// void redirect_io(char *input_file, char *output_file, int append)
// {
//     int input_fd, output_fd;

//     // Open input file if provided
//     if (input_file != NULL)
//     {
//         input_fd = open(input_file, O_RDONLY);
//         if (input_fd == -1)
//         {
//             perror(MAG);
//             perror("No such input file found!");
//             perror(COL_RESET);
//             return;
//         }
//         dup2(input_fd, STDIN_FILENO);

//         close(input_fd);
//     }

//     // Open output file
//     if (output_file != NULL)
//     {
//         if (append)
//         {
//             output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
//         }
//         else
//         {
//             output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
//         }
//         if (output_fd == -1)
//         {
//             perror(MAG);
//             perror("Failed to open output file");
//             perror(COL_RESET);
//             return;
//         }
//         dup2(output_fd, STDOUT_FILENO);
//         dup2(output_fd, STDERR_FILENO);
//         close(output_fd);
//     }
// }

// void redirect(char **args)
// {

//     char *input_file = NULL;
//     char *output_file = NULL;
//     int append = 0;

//     // Check for redirection operators
//     int i = 0;
//     while (args[i] != NULL)
//     {
//         if (strcmp(args[i], ">>") == 0)
//         {
//             append = 1;
//             output_file = args[i + 1];
//         }
//         else if (strcmp(args[i], ">") == 0)
//         {
//             append = 0;
//             output_file = args[i + 1];
//         }
//         else
//         {
//             append = 0;
//             output_file = NULL;
//         }
//         if (strcmp(args[i], "<") == 0)
//         {

//             input_file = args[i + 1];
//         }
//         i++;
//     }
//     // Tokenize the command

//     // Fork and execute the command

//     // Child process
//     redirect_io(input_file, output_file, append);
//     char **argv = (char **)malloc(4096 * sizeof(char *));
//     for (int i = 0; args[i] != NULL; i++)
//     {
//         if ((strcmp(args[i], ">>") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], "<") == 0))
//             break;
//         argv[i] = args[i];
//     }
//     if (strcmp("peek", argv[0]) == 0)
//     {
//         peek(argv);

//     }
//     else if (strcmp("warp", argv[0]) == 0)
//     {

//         warp(argv); // if return with error dont add to history

//     }
//     else if (strcmp("proclore", argv[0]) == 0)
//     {
//         if (!argv[1])
//             proclore("self");
//         else
//         {
//             proclore(argv[1]);
//         }

//     }
//     else if ((strcmp("pastevents", argv[0]) == 0) && (argv[1]))
//     {
//         if (strcmp("purge", argv[1]) == 0)
//             pastevents_purge();

//     }
//     else if (strcmp("pastevents", argv[0]) == 0)
//     {

//         read_command();

//     }
//     else if (strcmp("seek", argv[0]) == 0)
//     {
//         seek(argv);

//     }
//     else if (strcmp("fg", argv[0]) == 0)
//     {
//         fg(argv);

//     }
//     else if (strcmp("bg", argv[0]) == 0)
//     {
//         bg(argv);

//     }
//     else if (strcmp("activities", argv[0]) == 0)
//     {
//         activities();

//     }
//     else if (strcmp("ping", argv[0]) == 0)
//     {
//         ping(argv);

//     }
//     else
//     {
//         execvp(argv[0], argv);
//         // sh_exec(args,input);
//         perror(MAG);
//         perror("execvp");
//         perror(COL_RESET);
//     }
//     free(argv);
// }

#include "headers.h"

void redirect_io(char *input_file, char *output_file, int append)
{
    int input_fd, output_fd;

    // Open input file if provided
    if (input_file != NULL)
    {
        input_fd = open(input_file, O_RDONLY);
        if (input_fd == -1)
        {
            perror(MAG);
            perror("No such input file found!");
            perror(COL_RESET);
            return;
        }
        dup2(input_fd, STDIN_FILENO);

        close(input_fd);
    }

    // Open output file
    if (output_file != NULL)
    {
        if (append)
        {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        }
        else
        {
            output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (output_fd == -1)
        {
            perror(MAG);
            perror("Failed to open output file");
            perror(COL_RESET);
            return;
        }
        dup2(output_fd, STDOUT_FILENO);
        dup2(output_fd, STDERR_FILENO);
        close(output_fd);
    }
}

void redirect(char *input)
{

    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;

    // Tokenize the command

    char **args = split_line(input);
    int flag_out = 0;
    // Check for redirection operators

    for (int i = 0; args[i] != NULL; i++)
    {
        
        if (strcmp(args[i], ">") == 0) // there may be no i+1 then give error
        {
            if (flag_out)
            {
                printf("ERROR\n");
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
                printf("ERROR\n");
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
    // printf("%s %s\n", input_file, output_file);

    // Fork and execute the command
    int pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        redirect_io(input_file, output_file, append);
        char** argv=(char**)malloc(4096*sizeof(char*));
        for(int i=0;args[i]!=NULL;i++)
        {
            if((strcmp(args[i], ">>")==0)||(strcmp(args[i], ">")==0)||(strcmp(args[i], "<")==0))
                break;
            argv[i]=args[i];
        }
        if (strcmp("peek", argv[0]) == 0)
        {
            peek(argv);
            exit(1);
        }
        else if (strcmp("warp", argv[0]) == 0)
        {

            warp(argv); // if return with error dont add to history
            exit(1);
        }
        else if (strcmp("proclore", argv[0]) == 0)
        {
            if (!argv[1])
                proclore("self");
            else
            {
                proclore(args[1]);
            }
            exit(1);
        }
        else if ((strcmp("pastevents", argv[0]) == 0) && (argv[1]))
        {
            if (strcmp("purge", argv[1]) == 0)
                pastevents_purge();
            exit(1);
        }
        else if (strcmp("pastevents", argv[0]) == 0)
        {
            read_command();
           exit(1);
        }
        else if (strcmp("seek", argv[0]) == 0)
        {
            seek(argv);
            exit(1);
        }
        else
        {
            execvp(argv[0], argv);
            exit(1);
            // sh_exec(args,input);
            perror(MAG);
            perror("execvp");
            perror(COL_RESET);
        }
    }
    else
    {
        // Parent process

        wait(NULL);
    }
    if (output_file != NULL)
            {
                dup2(saved_stdout, 1);
                close(saved_stdout);
                dup2(saved_stderr, STDERR_FILENO);
                close(saved_stderr);
            }

            if (input_file != NULL)
            {
                dup2(saved_stdin, 0);
                close(saved_stdin);
            }
}