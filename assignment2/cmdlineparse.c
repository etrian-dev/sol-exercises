// Il programma deve riconoscere e stampare, se riconosciuti, i valori di 4 opzioni da riga di comando
// L'uso deve avere il seguente formato: ./<exe> -n <intero> -m <intero> -o <stringa> -h
// dove -h non prende argomenti e stampa soltanto l'help message
// Per effettuare il parsing di argv viene usata la funzione getopt()

#include <unistd.h> // per getopt
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Tutte le opzioni che getopt deve riconoscere (a parte -h) ':' se con attributo
#define GETOPT_STR ":n:m:o"

// definisce opzioni di help ed il messaggio da stampare
#define HELP_MSG "Usage: nome-programma -n <num. intero> -m <num. intero> -o <stringa> -h"

// funzione di utilità per determinare se la stringa *s possa essere convertita in intero
long isNumber(const char* s) {
    char* e = NULL;
    long val = strtol(s, &e, 0);
    if (e != NULL && *e == (char)0) return val;
    return -1;
}

// il main effettua il parsing degli argomenti passati da riga di comando
int main(int argc, char **argv) {
    // intero in cui trovo l'opzione riconosciuta
    int option;

    // sopprime stampa errori per opzioni non riconosciute
    opterr = 0;
    // Cerco opzione help negli argomenti: se la trovo stampo messaggio e termino prog
    do {
        option = getopt(argc, argv, "h");
        if(option == 'h') {
            puts(HELP_MSG);
            return EXIT_SUCCESS;
        }
    } while(option != -1);

    // Resetto optind per ricominciare il parsing (non avevo -h)
    optind = 1;
    // Abilita messaggi di errore per opzioni non riconosciute
    opterr = 1;

    long int res = -1;
    do {
        // getopt deve riconoscere le opzioni -n, -m, -o con argomento obbligatorio
        option = getopt(argc, argv, GETOPT_STR);
        // option == -1 se non riconosce l'opzione corrente

        // a seconda del valore di option stampa l'argomento
        switch(option) {
        case 'n':
            res = isNumber(optarg);
            if(res != -1) {
                printf("-%c: %ld\n", option, res);
            }
            break;
        case 'm':
            res = isNumber(optarg);
            if(res != -1) {
                printf("-%c: %ld\n", option, res);
            }
            break;
        case 'o':
            printf("-%c: %s\n", option, optarg);
            break;
        case ':':
        	printf("Opzione %c richiede un argomento\n", optopt);
        	break;
        case '?':
        	printf("Opzione %c non riconosciuta\n", optopt);	
        }
    } while(option != -1);

    return 0;
}
