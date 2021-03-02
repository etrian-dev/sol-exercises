#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

//	isNumber ritorna
//	0: ok
//	1: non e' un numbero
//	2: overflow/underflow
//
int isNumber(const char* s, long* n) {
  if (s==NULL) return 1;
  if (strlen(s)==0) return 1;
  char* e = NULL;
  errno=0;
  long val = strtol(s, &e, 10);
  if (errno == ERANGE) return 2;    // overflow
  if (e != NULL && *e == (char)0) {
    *n = val;
    return 0;   // successo 
  }
  return 1;   // non e' un numero
}

// funzione che calcola la somma dell'argomento con il valore dello stato interno
// e restituisce somma parziale
// NON RIENTRANTE (usa lo stato interno sotto forma della variabile statica accumulator)
int somma(int x) {
	static int accumulator;
	accumulator += x;
	return accumulator + INIT_VALUE;
}

int somma_r(int x, int *accum) {
	*accum += x;
	return *accum;
}

int main(int argc, char **argv) {
	long nreads = 0;
	if(argc == 2 && isNumber(argv[1], &nreads) == 0) {
		int i, x;
		int partial_sum = INIT_VALUE;
		for(i = 0; i < nreads; i++) {
			scanf(" %d", &x);
			printf("Somma parziale (non-reentrant): %d\n", somma(x));
			printf("Somma parziale (reentrant): %d\n", somma_r(x, &partial_sum));
		}
	}
}
