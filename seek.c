#include "headers.h"


int total_finds;
char e_path[PATH_MAX];//used to change directory (e flag)

int is_match(const char *name, const char *search_term) {
    int term_len = strlen(search_term);

    for (int i = 0; i <= term_len; i++) {
        if(i<term_len)
        {
        if (name[i] != search_term[i]) {
            return 0;
        }
        }
        else
        {
            if((name[i]!='\0')&&(name[i]!='.'))
                return 0;
        }
    }

    return 1;
}
void search_directory(int only_files,int only_dir,const int target,const char *dir_path, const char *search_term) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror(MAG);
        perror("opendir");
        perror(COL_RESET);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat entry_stat;
        if (stat(full_path, &entry_stat) == -1) {
            perror(MAG);
            perror("stat");
            perror(COL_RESET);
            continue;
        }
        
        if (S_ISDIR(entry_stat.st_mode)) {
            search_directory(only_files,only_dir,target,full_path, search_term);
        } 
        
        
         if (is_match(entry->d_name, search_term) != 0) {
            int flag=1;
            if((only_files)&&(S_ISDIR(entry_stat.st_mode)))
            {
               flag=0;
            }
            if((only_dir)&&(!S_ISDIR(entry_stat.st_mode)))
            {
                flag=0;
            }
            if(flag)
            {
                if(!S_ISDIR(entry_stat.st_mode))
                    printf(GRN);
                else
                    printf(BLU);
                total_finds++;
                strcpy(e_path,full_path);
                printf(".%s\n", full_path + target);//for length "target", only . is printed instead
                printf(COL_RESET);
            }
            
        }
    }

    closedir(dir);
}

int seek( char *args[]) {

    //tackling flags
    int e_flag=0;
    int l_flag=0;
    int d_flag=0;
    int i=1;
    if(!args[i])
    {
        printf(MAG);
        printf("seek: less arguments than required\n");
        printf(COL_RESET);
        return 1;
    }
    while((args[i])&&(args[i][0]=='-'))
    {
        if(strcmp(args[i],"-e")==0)
            e_flag=1;
        else if(strcmp(args[i],"-f")==0)
            l_flag=1;
        else if(strcmp(args[i],"-d")==0)
            d_flag=1;
        else if((strcmp(args[i],"-de")==0)||(strcmp(args[i],"-ed")==0))
            {
                d_flag=1;
                e_flag=1;
            }
         else if((strcmp(args[i],"-fe")==0)||(strcmp(args[i],"-ef")==0))
            {
                l_flag=1;
                e_flag=1;
            }
        else
            {
                printf(MAG);
                printf("Invalid flags!\n");
                printf(COL_RESET);
                return 1;
            }
        i++;
    }

     if(!args[i])
     {
        printf(MAG);
        printf("seek: less arguments than required\n");
        printf(COL_RESET);
        return 1;
    }
    
    if(d_flag && l_flag)
    {
        printf(MAG);
        printf("Invalid flags!\n");
        printf(COL_RESET);
        return 1;
    }

    //tackling target directory
    char *target_directory=(char*)malloc(sizeof(char)*1024);
    if(args[i+1])
   { 
    strcat(target_directory, "/");
    char delimit[] = "/"; // delimitors are /
    char *place = strtok(args[i+1], delimit);
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

            strcat(target_directory, HOME);
            strcat(target_directory, "/");
        }
        else if (strcmp(place, ".") == 0)
        {
            char curr_dir[PATH_MAX];
            getcwd(curr_dir,sizeof(curr_dir));
               strcat(target_directory, curr_dir);
            strcat(target_directory, "/");
        }
        else if(strcmp(place, "..") == 0)
        {
            char curr_dir[PATH_MAX];
            char parent_dir[PATH_MAX];
            getcwd(curr_dir,sizeof(curr_dir));
            chdir("..");
            getcwd(parent_dir,sizeof(parent_dir));
            chdir(curr_dir);
            strcat(target_directory, parent_dir);
            strcat(target_directory, "/");
        }
        else
        {
            strcat(target_directory, place);
            strcat(target_directory, "/");
        }

        place = strtok(NULL, delimit);
    }

    target_directory[strlen(target_directory)-1]='\0';
   }
   
    else
        target_directory = ".";
    // printf("%s\n",target_directory);
   
    

    //actual part
   
    const char *search_term = args[i];
    total_finds=0;
    search_directory(l_flag,d_flag,strlen(target_directory),target_directory, search_term);
    
    if(total_finds==0)
    {
        printf(MAG);
        printf("No match found!\n");
        printf(COL_RESET);
        return 0;
    }

    if(e_flag)
    {
        if(total_finds==1)
        {
            struct stat entry_stat;
            if (stat(e_path, &entry_stat) == -1) {
                perror(MAG);
                perror("stat");
                perror(COL_RESET);
                return 1;
            }


  
            if (S_ISDIR(entry_stat.st_mode))
            {
                // printf("only dir\n");
                if((entry_stat.st_mode & S_IXUSR) && (entry_stat.st_mode & S_IXGRP) && (entry_stat.st_mode & S_IXOTH))
                {
                    if(chdir(e_path)!= 0)
                    perror(MAG);
                    perror("warp: ");
                    perror(COL_RESET);
                    return 1;
                }   
                else
                {
                    printf(MAG);
                    printf("Missing permissions for task!\n");
                    printf(COL_RESET);
                    return 1;
                }
            }
            else
            {   
                //read file content
                 if(!((entry_stat.st_mode & S_IRUSR) && (entry_stat.st_mode & S_IRGRP) && (entry_stat.st_mode & S_IROTH)))
                {
                    printf(MAG);
                    printf("Missing permissions for task!\n");
                    printf(COL_RESET);
                    return 1;
                }   
                FILE *rfile = fopen(e_path, "r");
                if (!rfile)
                {
                    perror(MAG);
                    perror("Error opening file");
                    perror(COL_RESET);
                    return 1;
                }

                char str_buf[1000000]; //reading only 1e6 bytes of file to display on terminal
                while (fgets(str_buf, sizeof(str_buf), rfile) != NULL)
                {
                    printf("%s", str_buf);
                }
                printf("\n");
                fclose(rfile);
            }
        }
    }
 
    return 0;
}