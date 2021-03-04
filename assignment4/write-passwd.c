// Legge il file /etc/passwd e scrive in un file tutti i login name degli utenti
// Per il formato di /ect/passwd vedere man 5 passwd
// Il nome del file in cui scrivere viene passato come argomento al programma

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PWD_FILE "/etc/passwd"
#define MAX_LINE_SZ 512
#define ERROR_STR(file, ln, tm) "file: (file), line: (ln), time: (tm)"

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

    // read the file line by line and parse the line
    char line[MAX_LINE_SZ+1];
    char *username = NULL;
    char *delim = NULL;
    while(fgets(line, MAX_LINE_SZ+1, pwd_file) != NULL) {
      delim = strchr(line, ':');
      if(!delim) {
        puts(ERROR_STR(__FILE__, __LINE__, __TIME__));
        exit(EXIT_FAILURE);
      }

      // alloc memory for the username (using pointer arithmetic) and store it with strncpy
      username = malloc(sizeof(char)*(delim - line));
      if(!username) {
        perror(ERROR_STR(__FILE__, __LINE__, __TIME__));
        exit(EXIT_FAILURE);
      }
      strncpy(username, line, delim - line);
      // then write the username to the output file
      fprintf(out_file, "%s\n", username);

      // free the mem alloc'd for the username
      free(username);
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
