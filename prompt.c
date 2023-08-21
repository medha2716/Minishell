#include "headers.h"

void prompt()
{
    // USERNAME

    register struct passwd *pw;
    register uid_t uid;

    uid = geteuid();
    pw = getpwuid(uid);
    if (!pw)
        perror("Couldn't find UID!");
    char *username = pw->pw_name;
    

    // path
    char *path = calloc(PATHno_of_tokens_MAX, sizeof(char));
    if (!getcwd(path, PATHno_of_tokens_MAX))
        perror("Couldn't get the Working Directory");

    // HOSTNAME
    char hostname[513];
    if (gethostname(hostname, 512))
        perror("Couldn't find hostname!");

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
    printf("> ");
    printf(COL_RESET);
}
