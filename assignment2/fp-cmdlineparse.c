// Estende cmdlineparse.c con puntatori a funzione per gestione parametri
// Il programma deve riconoscere e stampare, se riconosciuti, i valori di 4 opzioni da riga di comando
// L'uso deve avere il seguente formato: ./<exe> -n <intero> -m <intero> -o <stringa> -h
// dove -h non prende argomenti e stampa soltanto l'help message
// Per effettuare il parsing di argv viene usata la funzione getopt()

#include <unistd.h> // per getopt
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Tutte le opzioni che getopt deve riconoscere (a parte -h) ':' se con attributo
#define GETOPT_STR ":n:m:o:h"
// definisce messaggio di uso
#define HELP_MSG "Usage: %s -n <num. intero> -m <num. intero> -o <stringa> -h\n"

// funzione di utilità per determinare se la stringa *s possa essere convertita in intero
int isNumber(const char* s, long int *num);

// definisco un tipo puntatore a funzione
typedef int (*PFun_t)(const char *);
// funzioni chiamate a seconda delle opzioni riconosciute
// definisco le funzioni di gestione delle opzioni
int arg_h(const char *str) {
    printf(HELP_MSG, str);
    return 0;
}
int arg_n(const char *str) {
    long int result;
    if(isNumber(str, &result) != -1) {
        printf("-n: %ld\n", result);
        return 0;
    }
    return -1;
}
int arg_m(const char *str) {
    long int result;
    if(isNumber(str, &result) != -1) {
        printf("-m: %ld\n", result);
        return 0;
    }
    return -1;
}
int arg_o(const char *str) {
    if(*str != '\0') {
        printf("-o: %s\n", str);
        return 0;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        puts("Nessun argomento specificato");
        return EXIT_SUCCESS;
    }

    // array di punatori a funzioni per gestione opzioni
    PFun_t V[4] = {arg_h, arg_m, arg_n, arg_o};
    // disabilito messaggio di errore getopt
    opterr = 0;
    int opt;
    while ((opt = getopt(argc,argv, GETOPT_STR)) != -1) {
        switch(opt) {
        case '?': {
            if(optarg == 0) {
                printf("Argomento -%c non riconosciuto\n", optopt);
            }
        }
        break;
        default:
        #ifdef DEBUG
        printf("opt = %c\noptarg = %s\n", opt, optarg);
        #endif
            // invocazione della funzione di gestione passando come parametro l'argomento restituito da getopt
            // NOTA: 'n'%4 == 0 ed m, n, o consecutivi nella asciitable
            if(V[opt%4]((optarg == NULL ? argv[0] : optarg)) == -1) {
                // gestione errore dovuto ad argomento mancante in una opzione
                if(opt == ':') {
                    printf("Argomento mancante all'opzione -%c\n", optopt);
                }
                else {
                  printf("L'argomento %s non è consentito per l'opzione -%c\n", optarg, opt);
                }
            }
        }
    }
    return 0;
}

int isNumber(const char* s, long int *num) {
    char* e = NULL;
    long val = strtol(s, &e, 0);
    if (
      (strncmp(s, "-1", 2) == 0)
      || (e != NULL && *e == (char)0))
    {
      *num = val;
      return 0;
    }
    return -1;
}
