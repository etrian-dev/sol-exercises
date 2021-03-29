
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* 
 * Calcola ricorsivamente il numero di Fibonacci dell'argomento 'n'.
 * La soluzione deve forkare un nuovo processo che esegue una sola 
 * chiamata di 'doFib'.
 * Se doPrint e' 1 allora la funzione lo stampa, altrimenti il
 * numero calcolato va passato al processo padre.
 */
static void doFib(int n, int doPrint);

int main(int argc, char *argv[])
{
	// questo programma puo' calcolare i numeri di Fibonacci solo fino a 13.
	const int NMAX = 13;
	int arg;

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <num>\n", argv[0]);
		return EXIT_FAILURE;
	}
	arg = atoi(argv[1]);
	if (arg <= 0 || arg > NMAX)
	{
		fprintf(stderr, "num deve essere compreso tra 1 e 13\n");
		return EXIT_FAILURE;
	}

	doFib(arg, 1);
	return 0;
}

static void doFib(int n, int doPrint)
{
	static int fib;
	static int fib_prev;

	if (n == 1)
	{
		fib_prev = 0;
		fib = 1;
	}
	else if (n == 2)
	{
		fib_prev = 1;
		fib = 1;
	}
	else
	{
		int child;
		if ((child = fork()) == -1)
		{
			perror("Cannot fork process");
			exit(-1);
		}

		// Il processo figlio chiama doFib una volta
		if (child == 0)
		{
			doFib(n - 1, 0);
		}
		else
		{
			int retcode;
			if (waitpid(child, &retcode, 0) == -1)
			{
				perror("Failed waiting");
				exit(-1);
			}
			if (WIFEXITED(retcode))
			{
				fib = WEXITSTATUS(retcode) + fib_prev;
				fib_prev = WEXITSTATUS(retcode);
			}
		}
	}

	// se doPrint == 1, stampa risultato
	if (doPrint == 1)
	{
		printf("Fib_%d = %d\n", n, fib);
	}
	else
	{
		exit(fib);
	}
}
