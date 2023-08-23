
#include "headers.h"
 
int check_if_pastevents(char* line)
{
    char* result=strstr(line,"pastevents");
    if(result==NULL)
        return 1;
    else
        return 0;
}
void remove_oldest_command()
{
    char bkpath[PATH_MAX];
    strcpy(bkpath,HOME);
    strcat(bkpath,"/");
    strcat(bkpath,BOOKKEEP_FILE_PATH);

    char tpath[PATH_MAX];
    strcpy(tpath,HOME);
    strcat(tpath,"/");
    strcat(tpath,TEMP_FILE_PATH);

    FILE *sourceFile = fopen(bkpath, "r");
    if (!sourceFile)
    {
        printf(MAG);
        printf("Error opening pastevents file\n");
        printf(COL_RESET);
        return;
    }

    FILE *tempFile = fopen(tpath, "w");
    if (!tempFile)
    {
        printf(MAG);
        printf("Error creating temporary file\n");
        printf(COL_RESET);
        fclose(sourceFile);
        return;
    }

    int lineNumber = 1;
    char line[5000]; //assuming each command is less than 4096 bytes

    while (fgets(line, sizeof(line), sourceFile))
    {
        // Skip the second line
        if (lineNumber != 2)
        {
            fputs(line, tempFile);
        }
        lineNumber++;
    }

    fclose(sourceFile);
    fclose(tempFile);

    // Replace the source file with the content of the temporary file
    remove(bkpath);
    rename(tpath, bkpath);
}
int add_command(char *command)
{
    int append_command = 0;
    // read no of commands and update it
     char bkpath[PATH_MAX];
    strcpy(bkpath,HOME);
    strcat(bkpath,"/");
    strcat(bkpath,BOOKKEEP_FILE_PATH);

    FILE *rfile = fopen(bkpath, "r+");
    if (!rfile)
    {
        printf(MAG);
        printf("Error opening pastevents file\n");
        printf(COL_RESET);
        return 1;
    }
    int currCount;
    fscanf(rfile, "%d", &currCount);
    if (currCount == 15)
    {
        remove_oldest_command();
        currCount--;
    }

    // Check if the command is the same as the last command in history
    char lastCommand[5000];      
    fseek(rfile, -2, SEEK_END); // Move before the last newline character
    while (ftell(rfile) > 0)
    {
        char ch = fgetc(rfile);
        if (ch == '\n')
        {
            break;
        }
        fseek(rfile, -2, SEEK_CUR);
    }
    fgets(lastCommand, sizeof(lastCommand), rfile);
    char currCommand[5000];
    strcat(currCommand, command);
    strcat(currCommand, "\n");
    // printf("%s %s\n",lastCommand,currCommand);
    // if (strcmp(lastCommand, currCommand) == 0)
    //     printf("same shit\n");

    if (strcmp(lastCommand, currCommand) != 0)
    {
        // beginning of file
        if (fseek(rfile, 0, SEEK_SET) != 0)
        {
            printf(MAG);
            printf("Error seeking to the beginning of pastevents file\n");
            printf(COL_RESET);
            fclose(rfile);
            return 1;
        }

        fprintf(rfile, "%d\n", currCount + 1);
        append_command = 1;
    }

    //
    fclose(rfile);

    if (append_command == 1)
    { // Write(append) commands to the file
        FILE *wfile = fopen(bkpath, "a");
        if (!wfile)
        {
            printf(MAG);
            printf("Error opening pastevents file\n");
            printf(COL_RESET);
            return 1;
        }
        fprintf(wfile, "%s\n", command);
        fclose(wfile);
    }

    return 0;
}

int pastevents_purge()
{
    char bkpath[PATH_MAX];
    strcpy(bkpath,HOME);
    strcat(bkpath,"/");
    strcat(bkpath,BOOKKEEP_FILE_PATH);
    FILE *rfile = fopen(bkpath, "w");
    if (!rfile)
    {
        printf(MAG);
        printf("Error opening pastevents file\n");
        printf(COL_RESET);
        return 1;
    }
    fprintf(rfile, "%d\n", 0);
    return 0;
}
int read_command()
{
    char bkpath[PATH_MAX];
    strcpy(bkpath,HOME);
    strcat(bkpath,"/");
    strcat(bkpath,BOOKKEEP_FILE_PATH);
    FILE *bitchworld = fopen(bkpath, "r");
    if (!bitchworld)
    {
        printf(MAG);
        printf("Error opening pastevents file\n");
        printf(COL_RESET);
        return 1;
    }
    char bitch[5000];
    int i = 0;
     fseek(bitchworld, 0, SEEK_SET);
    while (fgets(bitch, sizeof(bitch), bitchworld))
    {
        if (i == 1)
            printf("%s", bitch); 
        i = 1;
    }
    fclose(bitchworld);
    return 0;
}
void execute(char* line)
{
     check_bg_if_ended();
        // PASTEVENTS HISTORY!!!!!!!!!
        char line_copy[5000];
        strcpy(line_copy,line);
        line_copy[strlen(line_copy)-1]='\0';
        // int flag_add_to_history=0;
        // flag_add_to_history=check_if_pastevents(line_copy);
        // if(flag_add_to_history)
        //     add_command(line_copy);

        char **commands_separated_by_semicolon = sh_extract_commands(line);

        int i = 0;
        while (commands_separated_by_semicolon[i] != NULL)
        {
            char **tokens = sh_split_line(commands_separated_by_semicolon[i]); // specification 2
            i++;

            // printf("%s",tokens[0]);
            if (tokens[0] != NULL) // no command
            {
                sh_exec(tokens,line_copy);
            }

            free(tokens);
        } // free memory before next command

       
}
int execute_command(int command_position,char* to_be_added)
{
    char bkpath[PATH_MAX];
    strcpy(bkpath,HOME);
    strcat(bkpath,"/");
    strcat(bkpath,BOOKKEEP_FILE_PATH);
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
    int pos = currCount - command_position +2;
    // printf("%d\n",pos);
    char line[5000];
    int i = 1;
    while (fgets(line, sizeof(line), read_file))
    {
        // printf("%s", line);
        if (i == pos)
        {
            printf("%s", line); // dont print but execute but for now do this
            execute(line);
            break;
        }
        i++;
    }
    fclose(read_file);
    return 0;
}

