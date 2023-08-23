#ifndef __SEEK_H
#define __SEEK_H

int is_match(const char *name, const char *search_term);
void search_directory(int only_files,int only_dir,const int target,const char *dir_path, const char *search_term);
int seek( char *args[]);

#endif