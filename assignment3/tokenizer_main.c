#include "tokenizer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char* strndup(char *s, size_t sz);

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s [string1] [string2]\n", argv[0]);
	}
	else {
		char *s1 = strndup(argv[1], strlen(argv[1]));
		char *s2 = strndup(argv[2], strlen(argv[2]));
		puts("non-reentrant tokenizer");
		tokenizer(s1, s2);
		free(s1);
		free(s2);
		
		s1 = strndup(argv[1], strlen(argv[1]));
		s2 = strndup(argv[2], strlen(argv[2]));
		puts("non-reentrant tokenizer");
		tokenizer_r(s1, s2);
		free(s1);
		free(s2);
	}
	return 0;
}
