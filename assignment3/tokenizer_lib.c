#include "tokenizer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// declares these functions because they're not part of c99
char* strndup(const char *s, size_t n);
char* strdup(char *s);

// definisco strtok_r perché non fa parte di c99
char* strtok_r(char *s, const char *delim, char **saveptr);

void tokenizer(char *s1, char *s2) {
  char *tok1 = strtok(s1, " ");
  char *tok2 = NULL;
  while(tok1) {
    printf("%s\n", tok1);
    tok2 = strtok(s2, " ");
    while(tok2) {
      printf("%s\n", tok2);
      tok2 = strtok(NULL, " ");
    }
    tok1 = strtok(NULL, " ");
  }
}

void tokenizer_r(char *s1, char *s2) {
  char *s1_state = NULL;
  char *s2_state = NULL;

  char *tok1 = strtok_r(s1, " ", &s1_state);
  char *tok2 = NULL;
  char *s2_cpy = NULL;
  
  while(tok1) {
    printf("%s\n", tok1);
    
    
    s2_cpy = strndup(s2, strlen(s2));
  	if(!s2_cpy) {
  		puts("memory cannot be alloc'd");
  		exit(EXIT_FAILURE);
  	}
  	s2_state = NULL;
  	tok2 = strtok_r(s2_cpy, " ", &s2_state);
    while(tok2) {
      printf("%s\n", tok2);
      tok2 = strtok_r(NULL, " ", &s2_state);
    }
    free(s2_cpy);
    
    tok1 = strtok_r(NULL, " ", &s1_state);
  }
}
