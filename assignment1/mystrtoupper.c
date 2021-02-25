#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // toupper
#include <string.h>

void strtoupper(const char *in, size_t len, char *out) {
	unsigned int i;
	for(i = 0; i < len; i++) {
		out[i] = toupper(in[i]);
	}
	// non è indispensabile per via della calloc, ma non deve dipendere da come ho allocato
	// memoria
	out[len] = '\0';
}

int main(int argc, char **argv) {
	size_t i;
	for(i = 1; i < argc; i++) {
		size_t lenght = strlen(argv[i]);
		char *buf = calloc(lenght + 1, sizeof(char));
		if(!buf) {
			return EXIT_FAILURE;
		}
		strtoupper(argv[i], lenght, buf);
		printf("strtoupper(%s, %lu, %s)\n", argv[i], lenght, buf);
		free(buf);
	}
	
	return EXIT_SUCCESS;
}
