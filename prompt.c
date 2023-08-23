#include "headers.h"

void prompt()
{
    // USERNAME

    register struct passwd *pw;
    register uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);
    if (!pw)
    {
        perror(MAG);
        perror("Couldn't find UID!");
        perror(COL_RESET);
        return ;
    }
        
    char *username = pw->pw_name;
    

    // path
    char *path = calloc(PATH_MAX, sizeof(char));
    if (!getcwd(path, PATH_MAX))
    {
        perror(MAG);
        perror("Couldn't get the Working Directory");
        perror(COL_RESET);
        return ;
    }
        

    // HOSTNAME
    char hostname[5001];
    if (gethostname(hostname, 5000))
     {
        perror(MAG);
        perror("Couldn't find hostname!");
        perror(COL_RESET);
        return ;
    }
        

    printf(GREY);
    printf("<");
    printf(COL_RESET);
    printf(BLU);
    printf("%s", username);
    printf(COL_RESET);
    printf(GREY);
    printf("@");
    printf(COL_RESET);
    printf("%s", hostname);
    printf(GREY);
    printf(":");
    printf(COL_RESET);

    printf(GREY);
    
    if (strncmp(path, HOME, strlen(HOME)) == 0)
    {
        printf("~%s", path + strlen(HOME));
    }
    else
    {
        printf("%s", path);
    }
    printf(COL_RESET);
    // sleep : 5s
    if(time_flag>2)
    {
    printf(YEL);
    printf(" %s : %lds ",arg_0,time_flag);
    printf(COL_RESET);
    }

     printf(GREY);
    printf("> ");
    printf(COL_RESET);
}
