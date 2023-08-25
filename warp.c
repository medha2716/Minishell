#include "headers.h"

void print_path()
{
    char *path = calloc(PATH_MAX, sizeof(char));
    getcwd(path, PATH_MAX);
    printf("%s\n", path);
}
void warp(char **args) // what happens if warp - in the very beginning??
{

    char *CURPWD = calloc(PATH_MAX, sizeof(char));
    getcwd(CURPWD, PATH_MAX);

    if (args[1] == NULL)
    {
        // printf("hi");
        

        if (chdir(HOME) != 0)
        {
            perror(MAG);
            perror("warp: ");
            perror(COL_RESET);
        }
        else
        {
            print_path();
        }
        strcpy(OLDPWD, CURPWD);
        return;
    }
    int i = 1;
    // printf("%d\n",pos);
    while (args[i] != NULL)
    {
        //   printf("hi\n");
        char delimit[] = "/"; // delimitors are /
        char *place = strtok(args[i], delimit);
        int flag = 1;
        while (place != NULL)

        {
            //    printf("%s\n",place);
            if (strcmp(place, "~") == 0)
            {

                if (chdir(HOME) != 0)
                {
                    perror(MAG);
                    perror("warp: ");
                    perror(COL_RESET);
                    break;
                }

                flag = 0;
            }
            else if (strcmp(place, "-") == 0) // check
            {

                if (chdir(OLDPWD) != 0)
                {

                    perror(MAG);
                    perror("warp: ");
                    perror(COL_RESET);
                    break;
                }

                flag = 0;
            }
            else
            {

                if (chdir(place) != 0)
                {
                    perror(MAG);
                    perror("warp: ");
                    perror(COL_RESET);
                    // printf("%s %s %s\n",args[pos],args[pos+1],place);
                    break; // in a single path if at any point error come out eg.in "home/try hope" home/try is 1 path
                }

                flag = 0;
            }

            place = strtok(NULL, delimit);
        }
        if (flag == 0)
        {
            print_path();
        }
        i++;
    }
    strcpy(OLDPWD, CURPWD);
    return;
}
