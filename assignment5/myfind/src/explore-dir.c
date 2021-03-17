// Function to explore the directory tree rooted at the given path, searching for pattern

// my includes
#include <utilities.h>
#include <explore-dir.h>
// other std includes used
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
// to work with directories using libc calls (the DIR structure)
#include <dirent.h>
// system calls headers (stat and such)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// Opens and explores (recursively) the directory tree passed in
// 0 -> everything OK
// 1 -> can't open (some) directory
// 2 -> error in changing directory
// 3 -> error in exploring the directory
int explore_dir(const char *dir_path, const char *pattern) {
	#ifdef DEBUG
	printf("\n\n*** ENTERED DIRECTORY %s ***\n\n", dir_path);
	#endif
	
	// First change the current working dir to the one specified
	if(chdir(dir_path) == -1) {
		perror(dir_path);
		return 2;
	}
	
	// Try to open the directory
	DIR *directory = opendir(dir_path);
	if(!directory) {
		perror(dir_path);
		return 1;
	}
	
	// information about the directory is read into a structure by the readdir()
	// standard library call
	struct dirent *dir_info = NULL;
	int is_finished = 0; // flag telling all the directory has been explored
	
	// access all files in this directory (enters recursively in subdirectories)
	do {
		// readdir returns a (struct dirent*), but may return NULL 
		// even if there was no error (there are simply no more inodes to examine)
		// If so, errno shouldn't have been changed
		errno = 0;
		dir_info = readdir(directory);
		// If this condition is true, then an error occurred: do cleanup and return
		if(errno != 0 && !dir_info) {
			perror(dir_path);
			cleanup(directory);
			return 3;
		}
		// Otherwise the directory stream simply finished => exit the loop
		else if(!dir_info){
			is_finished = 1;
		}
		// Everything is fine: examine this inode with stat()
		else {
			// if the inode is "." or ".." ignore it
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
			
			// Find out whether this node is a regular file or a directory
			struct stat nodeinfo; // this struct holds info returned by the stat() syscall
			if(stat(apath, &nodeinfo) == -1) {
				// error occurred; report it and keep processing the directory
				perror(apath);
			}
			else {
				// If this is a directory => explore it (RECURSIVE!)
				if(S_ISDIR(nodeinfo.st_mode)) {	
					int ret_code = explore_dir(apath, pattern);
					err_report(ret_code, apath); // report an error if needed
				}
				// This is a regular file: find out whether the filename matches
				else if(S_ISREG(nodeinfo.st_mode)) {
					// The match needs to be exact (case-sensitive)
					if(strcmp(dir_info->d_name, pattern) == 0) {
						// Now display info about it:
						// absolute path, size and last modified date
						printf(
							"%s: %s\n%s: %-lu bytes\n%s: %s", 
							"FILE", 
							apath, 
							"SIZE",
							(unsigned long)nodeinfo.st_size,
							"LAST MODIFIED", 
							ctime(&(nodeinfo.st_mtime)));
					}
				}
			}
			
			// the string holding the abs path can now be freed
			free(apath);
		}
	} while(is_finished == 0);
	// This directory has been explored, and thus can be closed (cleanup is just a wrapper)
	cleanup(directory);
	
	// navigate back to the upper directory in the hierarchy ("..")
	if(chdir("..") == -1) {
		perror("Error returning from dir");
		return 2;
	}
	
	#ifdef DEBUG
	printf("\n\n*** EXITED DIRECTORY %s ***\n\n", dir_path);
	#endif
	
	// All went well: return 0
	return 0;
}
