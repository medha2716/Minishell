#include "headers.h"

 

// green for executables, white for files and blue for directories
// “.”, “..”, “~”,

int compare_entries(const void *a, const void *b)
{
    const struct dirent *entry_a = *(const struct dirent **)a;
    const struct dirent *entry_b = *(const struct dirent **)b;
    return strcmp(entry_a->d_name, entry_b->d_name);
}

void display_permissions(mode_t mode)
{

    printf((S_ISDIR(mode)) ? "d" : "-");
    printf((mode & S_IRUSR) ? "r" : "-");
    printf((mode & S_IWUSR) ? "w" : "-");
    printf((mode & S_IXUSR) ? "x" : "-");

    printf((mode & S_IRGRP) ? "r" : "-");
    printf((mode & S_IWGRP) ? "w" : "-");
    printf((mode & S_IXGRP) ? "x" : "-");

    printf((mode & S_IROTH) ? "r" : "-");
    printf((mode & S_IWOTH) ? "w" : "-");
    printf((mode & S_IXOTH) ? "x" : "-");

    printf(" ");
}

void print_name(char *name, struct stat file_stat)
{
    if (S_ISLNK(file_stat.st_mode))
    {
        // The file is a symbolic link
        printf(CYN);
    }
    else if (file_stat.st_mode & S_IXUSR)
    {
        // The file is executable
        printf(GRN);
    }
    else if (S_ISDIR(file_stat.st_mode))
    {
        // The file is a directory
        printf(BLU);
    }
    else
    {
        // normal
        printf(WHT);
    }

    printf(" %s\n", name);
    printf(COL_RESET);
    return;
}
int peek_print(const char *path, int show_hidden, int show_details)
{

    struct dirent *entry;
    struct dirent **entry_list = (struct dirent **)malloc(sizeof(struct dirent *) * 7000); // assuming max 7000 files in a directory
    DIR *dir = opendir(path);

    if (!dir)
    {
        printf(MAG);
        printf("No such file or directory\n");
        printf(COL_RESET);
        return 1;
    }
    int no_of_entries = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        entry_list[no_of_entries++] = entry;
        // printf("%s\n",(entry_list[no_of_entries-1]->d_name));
    }
    qsort(entry_list, no_of_entries, sizeof(struct dirent *), compare_entries);

    for (int j = 0; j < no_of_entries; j++)
    {
        if (show_hidden == 0) //-a flag not used
        {
            if (entry_list[j]->d_name[0] == '.') // file is hidden
                continue;
        }

        char full_path_to_entry[PATH_MAX];                                            // to get file details need to get file_stat
        snprintf(full_path_to_entry, PATH_MAX, "%s/%s", path, entry_list[j]->d_name); // path/entry_name

        struct stat file_stat;
        if (lstat(full_path_to_entry, &file_stat) < 0)
        {
            perror(MAG);
            perror("Error getting file stats");
            perror(COL_RESET);
            closedir(dir);
            return 1;
        }

        if (show_details) //-l used
        {
            char time_buf[2000];
            strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", localtime(&file_stat.st_mtime));

            struct passwd *owner = getpwuid(file_stat.st_uid);
            if (owner == NULL)
            {
                perror(MAG);
                perror("getpwuid");
                perror(COL_RESET);
                return 1;
            }

            struct group *group = getgrgid(file_stat.st_gid);
            if (group == NULL)
            {
                perror(MAG);
                perror("getgrgid");
                perror(COL_RESET);
                return 1;
            }

            display_permissions(file_stat.st_mode);
            printf("%lu %s %s %6ld %s", (unsigned long)file_stat.st_nlink, owner->pw_name, group->gr_name, (long)file_stat.st_size, time_buf);
        }
        print_name(entry_list[j]->d_name, file_stat);
    }

    closedir(dir);
    return 0;
}

int peek(char **args)
{
    char *path = calloc(PATH_MAX, sizeof(char));
    int show_hidden = 0;
    int show_details = 0;
    int start = 2;
    if (args[1])
    {
        if (args[1][0] == '-') // for flags
        {
            if ((strcmp(args[1], "-al") == 0) || (strcmp(args[1], "-la") == 0))
            {
                show_hidden = 1;
                show_details = 1;
            }
            else if (strcmp(args[1], "-a") == 0)
            {
                if ((args[2]) && (strcmp(args[2], "-l") == 0))
                {
                    show_hidden = 1;
                    show_details = 1;
                    start++;
                }
                else
                {
                    show_hidden = 1;
                    show_details = 0;
                }
            }
            else if (strcmp(args[1], "-l") == 0)
            {
                if ((args[2]) && (strcmp(args[2], "-a") == 0))
                {
                    show_hidden = 1;
                    show_details = 1;
                    start++;
                }
                else
                {
                    show_hidden = 0;
                    show_details = 1;
                }
            }
            else // "peek -"
            {
                printf(MAG);
                printf("No such file or directory\n");
                printf(COL_RESET);
                return 1;
            }

            // if flag path is from 2 or 3
        }
        else
        {
            start = 1;
        }
    }
    else
    {
        strcpy(path, ".");
        peek_print(path, show_hidden, show_details);
        return 0;
    }

    if (!args[start])
    {
        strcpy(path, ".");
        peek_print(path, show_hidden, show_details);
        return 0;
    }

    int i = start;
    strcat(path, "/");
    char delimit[] = "/"; // delimitors are /
    char *place = strtok(args[i], delimit);
    int flag = 1;
    while (place != NULL)

    {
        //    printf("%s\n",place);
        if (strcmp(place, "~") == 0)
        {
            // struct passwd *pw = getpwuid(getuid());
            // if ((!pw))
            // {
            //     printf("PWUID or UID not found!\n");
            //     return 1;
            // }

            strcat(path, HOME);
            strcat(path, "/");
        }
        else
        {
            strcat(path, place);
            strcat(path, "/");
        }

        place = strtok(NULL, delimit);
    }
    path[strlen(path) - 1] = '\0';
    peek_print(path, show_hidden, show_details);
    return 0;
}
