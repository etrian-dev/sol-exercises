#include <utilities.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h> // to work with directories using libc calls (the DIR structure)
#include <unistd.h>

void cleanup(DIR *pdir) {
	if(pdir) {
		if(closedir(pdir) == -1) {
			perror("Error closing directory");
			exit(EXIT_FAILURE);
		}
	}
}

void err_report(int status, const char *path) {
	if(status == 1) {
		printf("[ERROR] Error opening %s\n", path);
	}
	else if(status == 2) {
		printf("[ERROR] Error changing cwd to %s\n", path);
	}
	else if(status == 2) {
		printf("[ERROR] Error exploring %s\n", path);
	}
}

// function: reallocs a dynamically alloc'd string pointed to by s
// to the new size newsz. Sets status to 0 on success, 1 on failure
int realloc_str(char **s, size_t newsz) {
	char *buf = realloc(*s, newsz);
	if(!buf) {
		perror("Rellocation of buf at __FILE__:__LINE__");
		return 1;
	}
	*s = buf;
	return 0;
}
// Gets the absolute path for getting to file/dir name
// Works by appending to the current working directory's absolute path
// the string name (the memory is malloc'd, so it needs to be freed by the calling function)
// Return a pointer to the string containing the absoulte path (needs to be freed by the caller)
// or NULL on error (the caller should check the return value)
char* get_abspath(const char *name) {
	char *abspath = malloc(ALLOC_INC * sizeof(char));
	if(!abspath) {
		perror("abspath(): Allocating buffer");
		return NULL;
	}
	size_t apath_sz = ALLOC_INC;
	
	// try until the allocated buffer can fit the absolute path and the name
	char *result = NULL;
	int quit = 0;
	int ret;
	do {
		result = getcwd(abspath, apath_sz);
		// ERANGE means the the path to the cwd is longer than apath_sz bytes
		if(result == NULL && errno == ERANGE) {
			// realloc abspath
			
			ret = realloc_str(&abspath, apath_sz + ALLOC_INC);
			if(ret != 0) {
				// realloc failed
				free(abspath);
				return NULL;
			}
			apath_sz += ALLOC_INC;
		}
		else {
			quit = 1;
		}
	} while(quit == 0);
	
	#ifdef DEBUG
	printf("getabspath -> %s\n", abspath);
	#endif
	
	// Now abspath contains the absolute path to the current working directory
	// Append the filename to it
	size_t name_sz = strlen(name);
	ret = realloc_str(&abspath, apath_sz + name_sz + 2);
	if(ret != 0) {
		// realloc failed
		perror("abspath(): Reallocating a buffer");
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
