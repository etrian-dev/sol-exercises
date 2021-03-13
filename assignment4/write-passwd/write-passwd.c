// Legge il file /etc/passwd e scrive in un file tutti i login name degli utenti
// Per il formato di /ect/passwd vedere man 5 passwd
// Il nome del file in cui scrivere viene passato come argomento al programma

/* Command to compare the output to the right one (see man cut for infos)
 * Shouldn't output anything on success
 * ./write-passwd.out usernames.txt; cut -d ':' -f 1 /etc/passwd | diff - usernames.txt
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// The input file is the one defined below
#define PWD_FILE "/etc/passwd"
// assuming any line is shorter than MAX_LINE_SZ-1 characters
#define MAX_LINE_SZ 1024
// just an error logging macro
#define ERROR_STR(file, ln, tm) "Error at\nfile: (file)\nline: (ln)\ntime: (tm)"

// This program reads PWD_FILE and writes the first field (delimited by ':')
// in the file provided as an argument
int main(int argc, char **argv) {
  if(argc != 2) {
    printf("Usage: %s [outfile]\n", argv[0]);
  }
  else {
    // open the file /etc/passwd in read mode
    FILE *pwd_file = fopen(PWD_FILE, "r");
    if(!pwd_file) {
      perror(PWD_FILE);
      return EXIT_FAILURE;
    }
    // open the output file supplied as the first argument in write mode
    FILE *out_file = fopen(argv[1], "w");
    if(!out_file) {
      perror(argv[1]);
      return EXIT_FAILURE;
    }

    // read the file line by line (using a fixed buffer) and parse the line
    char line[MAX_LINE_SZ];
    char *delim = NULL;
    while(fgets(line, MAX_LINE_SZ, pwd_file) != NULL) {
      // the first field is delimited by a colon, thus the username 
      // terminates the character before that
      delim = strchr(line, ':');
      if(!delim) {
        puts(ERROR_STR(__FILE__, __LINE__, __TIME__));
        exit(EXIT_FAILURE);
      }
      // put a string terminator where the colon was
      size_t pos = delim - line;
      line[pos] = '\0';
      // then write the username to the output file
      // the line above implies that printf stops at the string terminator
      // this is useful because then the buffer needn't be resetted after each read
      fprintf(out_file, "%s\n", line);
    }

    // all the reading & writing done, files must be closed before exiting
    if(fclose(pwd_file) != 0) {
      perror(PWD_FILE);
      return EXIT_FAILURE;
    }
    if(fclose(out_file) != 0) {
      perror(argv[1]);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
