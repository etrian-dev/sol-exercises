#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // toupper
#include <string.h> // strlen

// Scrivere una funzione C che prende in input come primo argomento una stringa,
// come secondo argomento la lunghezza della stringa e restituisca nel terzo argomento
// la stessa stringa con tutti i sui caratteri in maiuscolo:
// void strtoupper(const char* in, size_t len, char* out);

// Scrivere il programma main di test per la funzione 'strtoupper' che prende la/le
// stringa/e da passare alla funzione come argomenti da linea di comando.
// Per convertire una lettera in maiuscolo si può usare 'toupper' (man 3 toupper).

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
	if(argc == 1) {
		// no args passed: print usage message
		printf("Usage: %s <str1> [, <strN>]\n", argv[0]);
	}
	else {
		size_t i;
		for(i = 1; i < argc; i++) {
			size_t lenght = strlen(argv[i]);
			char *buf = calloc(lenght + 1, sizeof(char));
			if(!buf) {
				perror("Cannot allocate memory");
				return EXIT_FAILURE;
			}

			// convert argv[i] to caps
			strtoupper(argv[i], lenght, buf);
			// print the converted string out stdout
			printf("strtoupper(%s) = %s\n", argv[i], buf);
			// free the buffer
			free(buf);
		}
	}

	return EXIT_SUCCESS;
}
