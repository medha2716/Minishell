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

void redirect(char **args)
{

    char *input_file = NULL;
    char *output_file = NULL;
    int append = 0;
   

    // Check for redirection operators
    int i=0;
    while(args[i]!=NULL)
{
    if (strcmp(args[i], ">>")==0)
    {
        append = 1;
        output_file = args[i+1];

        
    }
    else if (strcmp(args[i], ">")==0)
    {
        append = 0;
        output_file = args[i+1];
        
    }
    else
    {
        append = 0;
        output_file = NULL;
    }
    if (strcmp(args[i], "<")==0)
    {
        
        input_file = args[i+1];
    }
    i++;
}
    // Tokenize the command

    

    // Fork and execute the command
   
        // Child process
        redirect_io(input_file, output_file, append);
        char** argv=(char**)malloc(4096*sizeof(char*));
        for(int i=0;args[i]!=NULL;i++)
        {
            if((strcmp(args[i], ">>")==0)||(strcmp(args[i], ">")==0)||(strcmp(args[i], "<")==0))
                break;
            argv[i]=args[i];
        }
        // if (strcmp("peek", args[0]) == 0)
        // {
        //     peek(args);
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if (strcmp("warp", args[0]) == 0)
        // {

        //     warp(args); // if return with error dont add to history
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if (strcmp("proclore", args[0]) == 0)
        // {
        //     if (!args[1])
        //         proclore("self");
        //     else
        //     {
        //         proclore(args[1]);
        //     }
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if ((strcmp("pastevents", args[0]) == 0) && (args[1]))
        // {
        //     if (strcmp("purge", args[1]) == 0)
        //         pastevents_purge();
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if (strcmp("pastevents", args[0]) == 0)
        // {
        //     read_command();
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if (strcmp("seek", args[0]) == 0)
        // {
        //     seek(args);
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if(strcmp("fg", args[0]) == 0)
        // {
        //     fg(args);
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if(strcmp("bg", args[0]) == 0)
        // {
        //     bg(args);
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if(strcmp("activities", args[0]) == 0)
        // {
        //     activities();
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else if(strcmp("ping", args[0]) == 0)
        // {
        //     ping(args);
        //     if (output_file != NULL)
        //     {
        //         dup2(saved_stdout, 1);
        //         close(saved_stdout);
        //         dup2(saved_stderr, STDERR_FILENO);
        //         close(saved_stderr);
        //     }

        //     if (input_file != NULL)
        //     {
        //         dup2(saved_stdin, 0);
        //         close(saved_stdin);
        //     }
        // }
        // else
        // {
            
            execvp(argv[0], argv);
            // sh_exec(args,input);
            perror(MAG);
            perror("execvp");
            perror(COL_RESET);

          
        
    
}