#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
		fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
		return -1;
    }
    
    // puntatore da usare per mantenere lo stato in strtok_r
    char *strtok_state = NULL;
    
    char* token1 = strtok_r(argv[1], " ", &strtok_state);

    while (token1) {
    	// stampa il token ottenuto
		printf("%s\n", token1);
		
		// stampa tutti i token della seconda stringa
		char *strtok2_state = NULL;
		char* token2 = strtok_r(argv[2], " ", &strtok2_state);
		while(token2) {
			printf("%s\n", token2);
			token2 = strtok_r(NULL, " ", &strtok2_state);
		}
		token1 = strtok_r(NULL, " ", &strtok_state);
    }
    return 0;
}
