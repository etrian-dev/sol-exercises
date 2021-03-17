#ifndef UTILITIES_H
#define UTILITIES_H

#include <dirent.h> // to work with directories using libc calls (the DIR structure)
#include <stddef.h>

#define ALLOC_INC 16

int lsdir(const char *dir_path);
void cleanup(DIR *pdir);
void err_report(int status, const char *path);
int realloc_str(char **s, size_t newsz);
char* get_abspath(const char *name);

#endif
