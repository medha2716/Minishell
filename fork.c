

#include "headers.h"
int redirect_execute_command(int command_position)
{
    // add after exec of all pastevents execute is done
   
    char bkpath[PATH_MAX];
    strcpy(bkpath, HOME);
    strcat(bkpath, "/");
    strcat(bkpath, BOOKKEEP_FILE_PATH);
    FILE *read_file = fopen(bkpath, "r");
    if (!read_file)
    {
        printf(MAG);
        printf("Error opening pastevents file\n");
        printf(COL_RESET);
        return 1;
    }
    int currCount;
    fscanf(read_file, "%d", &currCount);
    int pos = currCount - command_position + 2;
    if (command_position > currCount)
    {
        printf(MAG);
        printf("ERROR: pastevents: No command at entered position!\n");
        printf(COL_RESET);
        fclose(read_file);
        return 1;
    }
    // printf("%d\n",pos);
    char line[5000];
    int i = 1;
    while (fgets(line, sizeof(line), read_file))
    {
        // printf("%s", line);
        if (i == pos)
        {
            // printf("%s", line); // dont print but execute but for now do this
            execute(line);
            break;
        }
        i++;
    }
    fclose(read_file);
    return 0;
}

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
    // printf("%s %s\n", input_file, output_file);

    // Fork and execute the command
    int pid = fork();
    if (pid == -1)
    {
        perror(MAG);
        perror("fork");
        perror(COL_RESET);
        exit(1);
    }
    else if (pid == 0)
    {
        // Child process
        redirect_io(input_file, output_file, append);
        char **argv = (char **)malloc(5000 * sizeof(char *));
        for (int i = 0; args[i] != NULL; i++)
        {
            if ((strcmp(args[i], ">>") == 0) || (strcmp(args[i], ">") == 0) || (strcmp(args[i], "<") == 0))
                break;
            argv[i] = args[i];
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
            else if (strcmp("execute", args[1]) == 0)
            {
                if (!args[2])
                {
                    printf(MAG);
                    printf("ERROR: pastevents execute: too few arguments\n");
                    printf(COL_RESET);
                    exit(1);
                }
                else
                {
                    int pos = atoi(args[2]);
                    if (pos < 1)
                    {
                        printf(MAG);
                        printf("ERROR: pastevents: No command at entered position!\n");
                        printf(COL_RESET);
                        exit(1);
                    }
                    // printf("%d\n",pos);
                    redirect_execute_command(pos);
                }
            }
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
        else if (strcmp("neonate", args[0]) == 0)
        {
            if (args[1] == NULL)
            {
                printf(MAG);
                printf("neonate: usage: neonate -n [time_arg]\n");
                printf(COL_RESET);
                exit(1);
            }
            if (args[2] == NULL)
            {
                printf(MAG);
                printf("neonate: usage: neonate -n [time_arg]\n");
                printf(COL_RESET);
                exit(1);
            }
            neonate(atoi(args[2])); // if not a number give error
            exit(1);
        }
        else if (strcmp("iMan", args[0]) == 0)
        {
            if (args[1] == NULL)
            {
                printf(MAG);
                printf("iMan: usage: iMan <command_name>\n");
                printf(COL_RESET);
                exit(1);
            }

            iman(args[1]); // if not a number give error
            exit(1);
        }
        else if (strcmp("activities", args[0]) == 0)
        {

            activities();
            exit(1);
        }
        else if (strcmp("fg", args[0]) == 0)
        {
            if (args[1] == NULL)
            {
                printf(MAG);
                printf("fg: usage: fg <pid>\n");
                printf(COL_RESET);
                exit(1);
            }
            fg(args);
            exit(1);
        }
        else if (strcmp("bg", args[0]) == 0)
        {
            if (args[1] == NULL)
            {
                printf(MAG);
                printf("bg: usage: bg <pid>\n");
                printf(COL_RESET);
                exit(1);
            }
            bg(args);
            exit(1);
        }
        else if (strcmp("ping", args[0]) == 0)
        {
            if (args[1] == NULL)
            {
                printf(MAG);
                printf("ping: usage: ping <pid> <signal_number>\n");
                printf(COL_RESET);
                exit(1);
            }
            if (args[2] == NULL)
            {
                printf(MAG);
                printf("ping: usage: ping <pid> <signal_number>\n");
                printf(COL_RESET);
                exit(1);
            }
            ping(args);
            exit(1);
        }
        else
        {
            if (execvp(argv[0], argv))
            {
                printf(MAG);
                printf("ERROR: '%s' is not a valid command\n", args[0]);
                printf(COL_RESET);
                exit(1);
            }
            exit(1);
        }
        free(argv);
    }
    else
    {
        // Parent process

        int status;
        waitpid(pid, &status, WUNTRACED);
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