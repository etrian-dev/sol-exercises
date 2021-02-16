/*
 * Il programma riceve in input una lista di interi e li somma (se possibile)
 * Se un argomento non può essere convertito a long int allora stampa messaggio di errore
 * e continua a convertire i restanti argomenti: se non ne può convertire nessuno restituisce
 * sum = 0
 * Esempio: ./a.out 5 7 5 2 6 => sum = 25
 */
#include <stdarg.h> // per variadic arguments
#include <errno.h> // per controllo variabile errno
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLEN 1000 // definisce massima lunghezza stringa di formato di print_dbg

/*
 * Funzione con un numero variabile di argomenti: stampa il messaggio secondo la stringa
 * di formato fmt (printf-like) e gli argomenti usati
 * NOTA: nessun controllo sul loro numero rispetto ai placeholder presenti in fmt
 */
void print_dbg(const char *fmt, ...);

/*
 * Funzione che converte una stringa arg in un long int (se converte con successo modifica
 * il valore puntato da num).
 * Conversione avvenuta con successo: ritorna 0
 * Errore di overflow/underflow: ritorna 1
 * Impossibile convertire interamente arg ad un valore intero: ritorna 2
 * (ad esempio ritorna 2 se arg="11x", mentre ritorna 1 se arg="999999999999999999999999")
 */ 
char is_num(const char *arg, long int *num);


// main: prende una lista di argomenti e stampa la loro somma su stdout
// (se riesce a convertire l'argomento, altrimenti lo ignora e passa al successivo) 
int main(int argc, char **argv) {
	if(argc < 2) {
		puts("No argument supplied");
	}
	else {
	    long int sum = 0;
	    long int n;
	    int i = 1, conv = 0;
	    char *is_neg = NULL; // flag per determinare se ho una stringa che rappresenta un numero negativo
	    // se is_neg != NULL allora negativo, altrimenti positivo
	    
	    // scorro tutti gli argomenti dati al programma: da argv[1] a argv[argc-1]
	    while(i < argc) {
	        // se tutto ok ritorna 0 e mette il risultato della conversione in n 
	        conv = is_num(argv[i], &n);
	        switch(conv) {
	            case 0: // tutto ok
	                sum += n; break;
	            case 1: // overflow nella conversione
	                // determino se overflow o underflow: se underflow allora la stringa deve
	                // contenere il carattere '-'
	                is_neg = strchr(argv[i], '-');
	                // stampa messaggio di debug passando l'argomento ed errno
	                print_dbg(
	                    "[%s] \'%s\' is outside the range: %s\n", 
	                    "MAIN", 
	                    argv[i],
	                    (is_neg == NULL ? "overflow" : "underflow"));
	                break;
	            case 2: // argomento non numerico
	                // stampa messaggio di debug
	                print_dbg("[%s] \'%s\' cannot be converted to an integer\n", "MAIN", argv[i]); break;
	            default:
	                // stampa messaggio di debug per valore di ritorno is_num non riconosciuto
	                print_dbg("return value unrecognized");
	        }
	        // allora passo al prossimo argomento della lista
	        i++;
	    }
	    printf("%s => sum = %ld\n", argv[0], sum);
	}
	return 0;
}

void print_dbg(const char *fmt, ...) {
    va_list args; // declare an arg list
    va_start(args, fmt); // initializes the arg list
    
    char *msg = calloc(MAXLEN, sizeof(char));
    if(!msg) {
        puts("Cannot alloc memory");
        exit(EXIT_FAILURE);
    }
    
    vsnprintf(msg, MAXLEN, fmt, args);
    fputs(msg, stderr);
    
    free(msg);
    va_end(args); // "frees" the argument list
    
}

char is_num(const char *arg, long int *num) {
    // indirizzo del primo carattere di arg che non può essere convertito
    char *invalid_char = NULL;
    // setto errno a 0 per poi controllare se cambia per overflow
    errno = 0;
    // prova a convertire a base 10 arg
    long int res = strtol(arg, &invalid_char, 10);
    if(errno == ERANGE) {
        // allora ho avuto overflow o underflow
        return 1;
    }
    if(invalid_char != NULL && *invalid_char == '\0') {
        // allora strtol ha convertito correttamente => setto num
        *num = res;
        return 0;
    }
    if(invalid_char != NULL) {
        // altrimenti arg non era un numero: stampo ind del primo carattere non valido
        print_dbg(
            "[%s] \'%s\' cannot be entirely converted: first invalid character at %p\n", 
            "IS_NUM", 
            arg, 
            *invalid_char);
    }
    return 2;
}
