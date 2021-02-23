// Il programma deve riconoscere e stampare, se riconosciuti, i valori di 4 opzioni da riga di comando
// L'uso deve avere il seguente formato: ./<exe> -n <intero> -m <intero> -o <stringa> -h
// dove -h non prende argomenti e stampa soltanto l'help message
// Per effettuare il parsing di argv viene usata la funzione getopt()

#include <unistd.h> // per getopt
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Tutte le opzioni che getopt deve riconoscere. ':' se con attributo
#define GETOPT_STR "n:m:o:"

// definisce flag di stampa help (disabilita la stampa delle altre)
#define HELP_STR "hH"
#define HELP_MSG "Usage: nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h"

long isNumber(const char* s) {
   char* e = NULL;
   long val = strtol(s, &e, 0);
   if (e != NULL && *e == (char)0) return val; 
   return -1;
}

int main(int argc, char **argv) {
	// sopprime errori per opzioni non riconosciute
	opterr = 0;
	 
	int option;
	// Prova ad individuare opzione di help
	do {
		option = getopt(argc, argv, HELP_STR);
		if(strchr(HELP_STR, option) != NULL) {
			puts(HELP_MSG);
			return 0;
		}
	} while(option != -1);
	
	// Resetto optind per ricominciare il parsing
	optind = 1;
	// Abilita messaggio di errore per opzioni non riconosciute
	opterr = 1;
	
	// variabili di utilità per temporanei
	long int res = -1;
	do {
		// getopt deve riconoscere le opzioni -n, -m, -o con argomento obbligatorio
		// e l'opzione h (senza argomenti)
		option = getopt(argc, argv, GETOPT_STR);
		// getopt ritorna -1 se non riconosce l'opzione corrente
		
		// Se riconosce l'opzione corrispondente, salva il valore nella corrispondente
		// cella di optvals
		switch(option) {
			case 'n':
				res = isNumber(argv[optind]);
				if(res != -1) {
					printf("%s: %ld\n", optarg, res);
				}
				break;
			case 'm':
				res = isNumber(argv[optind]);
				if(res != -1) {
					printf("%s: %ld\n", optarg, res);
				}
				break;
			case 'o':
				printf("%s: %s\n", optarg, argv[optind]);
				break;
		}
	} while(option != -1);
	
	return 0;
}
