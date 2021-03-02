#include <stdio.h>
#include <stdlib.h>

#define dimN 16
#define dimM 8

// macro che prende due argomenti: un puntatore ptr ed una stringa s
// se ptr == NULL, stampa s con perror ed esce (EXIT_FAILURE)
// se ptr != NULL stampa il successo della malloc
#define CHECK_PTR_EXIT(ptr, s) 	\
	if((ptr) == NULL) {			\
		perror((s));			\
		exit(EXIT_FAILURE);		\
	}							\
	else {						\
		perror((s));			\
	}

// macro che ritorna l'elemento matr[i][j]
// Dal momento che la matrice è allocata come un unico array per righe e conosco
// la dimensione delle righe a tempo di compilazione, posso usare la seguente formula
#define ELEM(matr, i, j) *((matr) + (i) * dimM + (j))

#define MSG "Stampo la matrice M"
// macro che stampa la matrice matr, di dimensioni n x m
// usa la macro ELEM per accedere agli elementi a_ij
#define PRINTMAT(matr, n, m)					\
	int r, c;									\
	puts(MSG);									\
	for(r = 0; r < (n); r++) {					\
		for(c = 0; c < (m); c++) {				\
			printf("\t%ld", ELEM((matr), r, c));\
		}										\
		putchar('\n');							\
	}

int main() {
	// alloca una matrice di long ints di dimensioni dimN x dimM
	long *M = malloc(dimN * dimM * sizeof(long));
	// controlla che la malloc non abbia fallito
    CHECK_PTR_EXIT(M, "malloc"); 
    
    for(size_t i=0;i<dimN;++i)
		for(size_t j=0;j<dimM;++j)			
	    	ELEM(M,i,j) = i+j;    
    
    PRINTMAT(M, dimN, dimM);
    
    free(M);
    return 0;
}
