// programma che riconosce e stampa le opzioni
// -n <numero>
// -s <stringa>
// -m <altro numero>
// -h (messaggio help)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

void printNum(char opt, char *rest, char *next_token) {
	if(strcmp(rest, "") != 0 && isNumber(rest) != -1) {
		printf("-%c: %s\n", opt, rest);
	}
	else if(next_token != NULL && isNumber(next_token) != -1) {
		printf("-%c: %s\n", opt, next_token);
	}
	else {
		printf("-%c: No values provided!\n", opt);
	}
}

int main(int argc, char **argv) {
	if(argc < 2) {
		puts("Nessun argomento");
	}
	else {
		// faccio parsing degli argomenti
		int i;
		bool help = false;
		for(i = 1; i < argc; i++) {
			if(strstr(argv[i], "-h") != NULL) {
				help = true;
				printf("HELP\nUsage: %s [-n|-m|-s|-h] [argument]\n", argv[0]);
			}
		}
		i = 1;
		char *opt;
		while(!help && i < argc) {
			// if the i-th argument is not a number (according to isNumber) then skip the iteration
			// or if there's no '-' character
			if(isNumber(argv[i]) == -1 && strchr(argv[i], '-') != NULL) {
				// try to recognize n, m or s
				if((opt = strstr(argv[i], "-n"))) {
					printNum('n', opt + 2, argv[i+1]);
				}
				else if((opt = strstr(argv[i], "-m"))) {
					printNum('m', opt + 2, argv[i+1]);
				}
				else if((opt = strstr(argv[i], "-s"))) {
					if(*(strchr(argv[i], 's') + 1) != '\0') {
						printf("-s: %s\n", opt + 2);
					} else if(argv[i+1]) {
						printf("-s: %s\n", argv[i+1]);
					}
					else {
						printf("-s: no arguments\n");
					}
				}
				// option not recognized
				else {
					printf("%s: Opzione non riconosciuta\n", argv[i]);
				}
			}
			i++;
		}
	}
	return EXIT_SUCCESS;
}
