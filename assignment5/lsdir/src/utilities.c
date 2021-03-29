// Implementation of various utility functions defined in utilities.h

// my includes
#include <utilities.h>
// std lib includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h> // to work with directories using libc calls (the DIR structure)
#include <unistd.h>

// closes the directory pointed to by pdir, exiting if it fails to do so
void cleanup(DIR *pdir) {
	if(pdir) {
		if(closedir(pdir) == -1) {
			perror("Closing directory");
			exit(EXIT_FAILURE);
		}
	}
}

// reports errors that may have occurred in lsdir
void err_report(int status, const char *path) {
	if(status == 1) {
		printf("[ERROR] Error opening %s\n", path);
	}
	else if(status == 2) {
		printf("[ERROR] Error changing cwd to %s\n", path);
	}
	else if(status == 3) {
		printf("[ERROR] Error exploring %s\n", path);
	}
}

// reallocs a dynamically alloc'd string pointed to by s
// to the size newsz. Returns 0 on success, 1 on failure
int realloc_str(char **s, size_t newsz) {
	char *buf = realloc(*s, newsz);
	if(!buf) {
		perror("utilities.c: realloc_str()");
		return 1;
	}
	*s = buf;
	return 0;
}

// This function gets the absolute path to name
// Returns: NULL if name was NULL; the string containing the absolute path to name otherwise
char* get_abspath(const char *name) {
	// determine if the string is significative
	if(!name) {
		return NULL;
	}
	
	char *abspath = NULL;
	size_t apath_sz = ALLOC_INC;
	// check whether name is already an absolute path
	if(*name == '/') {
		// then duplicate it and return the string
		abspath = strdup(name, strlen(name) + 1);
	}
	else {
		// realloc the buffer until it can fit the absolute path and the name
		char *result = NULL;
		int quit = 0;
		int ret;
		do {
			result = getcwd(abspath, apath_sz);
			// ERANGE means the the path to the cwd is longer than apath_sz bytes
			if(result == NULL && errno == ERANGE) {
				ret = realloc_str(&abspath, apath_sz + ALLOC_INC);
				if(ret != 0) {
					// realloc failed
					free(abspath);
					perror("utilities.c: abspath() (realloc error)");
					return NULL;
				}
				apath_sz += ALLOC_INC;
			}
			else {
				quit = 1;
			}
		} while(quit == 0);
	}
	if(!abspath) {
		perror("utilities.c: abspath()");
		return NULL;
	}
	
	
	
	
	// Now abspath contains the absolute path to the current working directory
	// Append the filename to it
	size_t name_sz = strlen(name);
	ret = realloc_str(&abspath, apath_sz + name_sz + 2);
	if(ret != 0) {
		// realloc failed
		perror("utilities.c: abspath()");
		free(abspath);
		return NULL;
	}
	size_t apath_len = strlen(abspath);
	abspath[apath_len] = '/';
	abspath[apath_len + 1] = '\0';
	apath_sz += name_sz + 2;
	abspath = strncat(abspath, name, name_sz);
	#ifdef DEBUG
	printf("abs path to %s: %s\n", name, abspath);
	#endif
	
	return abspath;
}
