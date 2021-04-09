#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
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
	int fib = 0;

	// base case 1: Fib_1 = 1
	if (n == 1)
	{
		fib = 1;
	}
	// base case 2: Fib_2 = 1
	else if (n == 2)
	{
		fib = 1;
	}
	else
	{
		// fork two children: one calculates Fib_n-1 and the other Fib_n-2
		pid_t child1, child2;
		
		if ((child1 = fork()) == -1)
		{	if(errno == EAGAIN) {
				printf("[%d] Too many processes: can\'t fork Fib_%d\n", getpid(), n-1);
			}
		}
		if ((child2 = fork()) == -1)
		{
			if(errno == EAGAIN) {
				printf("[%d] Too many processes: can\'t fork Fib_%d\n", getpid(), n-2);
			}
		}

		// the first child calculates Fib_n-1
		if (child1 == 0)
		{
			doFib(n - 1, 0);
		}
		// the second child calculates Fib_n-2
		else if(child2 == 0) {
			doFib(n - 2, 0);
		}
		// the parent process waits for its two children to terminate and saves their
		// return code to extract the corresponding term of the sequence
		else
		{
			pid_t rpid1, rpid2;
			int retcode; // contains the fibonacci number
			
			// waits for child2's termination, then adds Fib_n-2 to fib
			if((rpid2 = waitpid(child2, &retcode, 0)) == -1) {
				perror("Cannot wait for process termination");
				exit(EXIT_FAILURE);
			}
			else if (WIFEXITED(retcode))
			{
				//printf("[%d] Process %d returned with status %d\n", getpid(), (int)rpid2, WEXITSTATUS(retcode));
				//printf("[%d] fib = %d\n", getpid(), fib + WEXITSTATUS(retcode));
				//fflush(stdout);
				fib += WEXITSTATUS(retcode);
			}
			
			// waits for child1's termination, then adds Fib_n-1 to fib
			if((rpid1 = waitpid(child1, &retcode, 0)) == -1) {
				perror("Cannot wait for process termination");
				exit(EXIT_FAILURE);
			}
			else if (WIFEXITED(retcode))
			{
				//printf("[%d] Process %d returned with status %d\n", getpid(), (int)rpid1, WEXITSTATUS(retcode));
				//printf("[%d] fib = %d\n", getpid(), fib + WEXITSTATUS(retcode));
				//fflush(stdout);
				fib += WEXITSTATUS(retcode);
			}
		}
	}

	// se doPrint == 1, stampa risultato
	if (doPrint == 1)
	{
		printf("[%d] Fib_%d = %d\n", getpid(), n, fib);
	}
	// altrimenti esce restituendo Fib_n al processo padre
	else
	{
		exit(fib);
	}
}
