// This program receives as args a directory path and then a filename and searches
// recursively into dir the files having that filename
// For each match, it prints the absolute path and the last modified date

#include <dirent.h> // to work with directories using libc calls (the DIR structure)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
// system calls headers for stat, ...
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char *strndup(const char *, size_t);

void print_usage(const char *fname) {
	printf("Usage: %s <dir> <fname>\n", fname);
}
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
#define ALLOC_INC 16
char* get_abspath(const char *name) {
	char *abspath = malloc(ALLOC_INC * sizeof(char));
	if(!abspath) {
		perror("Allocation of buf at __FILE__:__LINE__");
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

// opens and explores (recursively) the directory path passed in
// 0 -> everything OK
// 1 -> can't open dir
// 2 -> error exploring the dir (either changing directory or exploring files)
int explore_dir(const char *dir_path, const char *pattern) {
	#ifdef DEBUG
	printf("Exploring dir %s\n", dir_path);
	#endif
	
	// first change the current working dir to the one specified + error checking
	if(chdir(dir_path) == -1) {
		perror(dir_path);
		return 2;
	}
	
	// Try to open the directory passed in as the first argument
	DIR *directory = opendir(dir_path);
	if(!directory) {
		perror(dir_path);
		return 1;
	}
	
	// information about the directory is read into a structure by the following 
	// standard library call. It may return NULL even when no error occurs, but
	// in that case errno does not change, so setting it to a known value enables
	// checking for errors
	struct dirent *dir_info = NULL;
	int is_finished = 0;
	
	// access all files in this directory (descends recursively in subdirectories)
	do {
		// readdir returns a DIR*, but may return NULL even if there was no error
		// If so, errno shouldn't have been changed
		errno = 0;
		dir_info = readdir(directory);
		// If this condition is true, then an error occurred: cleanup and return it
		if(!dir_info && errno != 0) {
			perror(dir_path);
			cleanup(directory);
			return 3;
		}
		// Otherwise the directory stream simply finished => exit the loop
		else if(!dir_info){
			is_finished = 1;
		}
		// Everything is fine: find out whether the filename matched
		else {
			// if the directory is "." or ".." ignore them
			if(strcmp(".", dir_info->d_name) == 0 || strcmp("..", dir_info->d_name) == 0) {
				continue;
			}
			
			// stat needs the absoulte path to the file, so this code below retrieves it
			// NOTE: allocs memory that needs to be freed
			char *apath = get_abspath(dir_info->d_name);
			if(!apath) {
				cleanup(directory);
				return 3;
			}
			
			// find out whether this node is a regular file or a directory
			struct stat nodeinfo; // this struct holds info returned by the stat() syscall
			if(stat(apath, &nodeinfo) == -1) {
				// error occurred; report it and keep processing the directory's contents
				perror(apath);
			}
			else {
				// this is a directory => explore it (RECURSIVE!)
				if(S_ISDIR(nodeinfo.st_mode)) {
					#ifdef DEBUG
					printf("\n\n***ENTERING DIRECTORY %s***\n\n", dir_info->d_name);
					#endif
									
					// explore the subdirectory
					int ret_code = explore_dir(apath, pattern);
					err_report(ret_code, apath);
				}
				// this is a regular file: find out whether the filename matches
				else if(S_ISREG(nodeinfo.st_mode)) {
					#ifdef DEBUG
					printf("Regular file %s\n", dir_info->d_name);
					#endif
					
					// the strstr tries to find an occurrence of the second argument to the program
					if(strcmp(dir_info->d_name, pattern) == 0) {
						// Now display info about it: the absolute path and the last modified date
						printf(
							"%s: %s\n%s: %s", 
							"FILE", 
							apath, 
							"LAST MODIFIED", 
							ctime(&(nodeinfo.st_mtime)));
					}
				}
			}
			
			// the string holding the abs path can now be freed
			free(apath);
		}
	} while(is_finished == 0);
	
	// the search is completed, close the directory
	cleanup(directory);
	// navigate back to the cwd where this was called from ("..")
	if(chdir("..") == -1) {
		perror("Error returning from dir");
		return 2;
	}
	#ifdef DEBUG
	printf("\n\n***EXITED DIRECTORY %s***\n\n", dir_path);
	#endif
	
	// All went well: return 0
	return 0;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		print_usage(argv[0]);
	}
	else {
		// First print info about the source dir and the output table header
		printf("Searching for files matching \"%s\"\nin directory \"%s\"\n", argv[2], argv[1]);
			
		char *abspath = NULL;
		// argv[1] is already an absoulute path
		if(argv[1][0] == '/') {
			abspath = argv[1];
		}
		// gets the absolute path to argv[1]
		else {
			abspath = get_abspath(argv[1]);
		}
		// explore the directory
		int status = explore_dir(abspath, argv[2]);
		err_report(status, argv[1]);
		free(abspath);
	}
	return EXIT_SUCCESS;
}
