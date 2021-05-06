/*
 * Scrivere un programma C che conta il numero di volte in cui l'utente invia il segnale
 * SIGINT (Ctl-C) al processo. Quando il processo riceve il segnale SIGTSTP (Ctl-Z),
 * il numero di SIGINT ricevuti viene stampato sullo standard output. Al terzo segnale
 * SIGTSTP, il processo chiede all'utente se deve terminare o no attendendo una risposta
 * per 10 secondi dallo standard input. Se l'utente non risponde entro 10 secondi un
 * segnale SIGALRM termina il processo.
 *
 * reset count CTRL-C ogni volta che riceve CTRL-Z
 */

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    // disable SIGINT, SIGTSPT
    sigset_t mask;
    if(sigemptyset(&mask) == -1) {
	perror("Failed to set the mask");
	return 1;
    }
    // set SIGINT and SIGTSTP to be masked to wait for them
    if(sigaddset(&mask, SIGINT) == -1 || sigaddset(&mask, SIGTSTP) == -1) {
	perror("Failed to set the mask");
	return 1;
    }

    if(pthread_sigmask(SIG_BLOCK, &mask, NULL) == -1) {
	perror("Failed to set the mask");
	return 1;
    }

    // counter for stop and interrupt signals
    volatile sig_atomic_t count_interrupt = 0;
    volatile sig_atomic_t count_stop = 0;

    // now begin counting interrupts and stops
    int signal;
    while(count_stop < 3) {
	if(sigwait(&mask, &signal) != 0) {
	    perror("Failed to wait for signals");
	    return 2;
	}
	// if SIGINT, then increment its counter
	if(signal == SIGINT) {
	    count_interrupt++;
	}
	// otherwise it's a stop: print the #interrupts received up to now and do other stuff
	else {
	    printf("Ricevuto %d SIGINT\n", count_interrupt);
	    count_interrupt = 0;
	    count_stop++;
	}
    }
    // third stop signal received: the process asks with a timeout if the user wants to
    // terminate it. If the user replies "y" (on stdin) then continues, otherwise stops
    // if after ten seconds the user didn't reply, the process is terminated by a SIGALARM
    char repl[10];


    printf("Terminate the process\? [Y/n]: ");
    fflush(stdout);

    // alarm set to 10 seconds
    alarm(10);

    int ret = read(0, repl, 10);
    repl[1] = '\0';
    if(ret != -1 && strcmp(repl, "n") == 0) {
	printf("Ok, process will not terminate\n");
	fflush(stdout);
	// if the user replied no, then reset the alarm
	alarm(0);
	while(1);
    }

    return 0;
}
