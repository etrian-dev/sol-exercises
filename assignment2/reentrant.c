#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// declares these functions because they're not part of c99
char* strndup(const char *s, size_t n);
char* strtok_r(char *s, const char *delim, char **saveptr);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "use: %s stringa1 stringa2\n", argv[0]);
        return -1;
    }

    // puntatori da usare per mantenere lo stato di strtok_r
    char *strtok1_state = NULL;
    char *strtok2_state = NULL;
    char* token1 = strtok_r(argv[1], " ", &strtok1_state);
    char *str2 = NULL; 
    while (token1) {
        // stampa il token ottenuto dalla prima stringa
        printf("%s\n", token1);

        // stampa tutti i token della seconda stringa
        strtok2_state = NULL;
        // the string must be duplicated, since it's modified by strtok
        str2 = strndup(argv[2], strlen(argv[2]));
        if(!str2) {
        	puts("Memory cannot be alloc'd -> EXIT");
        	exit(-1); // brutal process exit
        }
        
        char* token2 = strtok_r(str2, " ", &strtok2_state);
        while(token2) {
            printf("%s\n", token2);
            token2 = strtok_r(NULL, " ", &strtok2_state);
        }
        token1 = strtok_r(NULL, " ", &strtok1_state);
        // free del duplicato della stringa
        free(str2);
    }
    return 0;
}
