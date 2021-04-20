/*
 * Scrivere un programma (chiamato pipedec) che prende in ingresso un intero positivo.
 * pipedec lancia 2 processi figli ognuno dei quali esegue con una chiamata exec*
 * il programma dec. Solo ad uno dei due processi figli viene passato come argomento
 * l'intero positivo passato al programma pipedec. I due processi figli devono essere
 * connessi tra di loro in modo tale che lo standard input di un processo sia connesso
 * con lo standard output dell'altro processo (la connessione e' quindi bidirezionale)
 */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// std lib headers
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv) {
    if(argc == 2) {
        // tramite pipe connette stdout di ch1 a stdin di ch2
        int p1[2];
        if(pipe(p1) == -1) {
            perror("Cannot create pipe");
            return 2;
        }
        // tramite pipe connette stdout di ch2 a stdin di ch1
        int p2[2];
        if(pipe(p2) == -1) {
            perror("Cannot create pipe");
            return 2;
        }

        // forka 2 processi
        pid_t ch1, ch2;

        if((ch1 = fork()) == -1) {
            perror("Cannot fork ch1");
            return 1;
        }
        if((ch2 = fork()) == -1) {
            perror("Cannot fork ch2");
            return 1;
        }

        // code exec by ch1: gets x
        if(ch1 == 0) {
            if(dup2(p1[0], 0) == -1) {
                perror("Cannot connect ch1\'s stdin");
            }
            if(dup2(p2[1], 1) == -1) {
                perror("Cannot connect ch1\'s stdout");
            }

            execl("./dec.out", "./dec.out", argv[1], NULL);

            perror("Exec failed");
            return 1;
        }
        // code exec by ch2: doesn't get x
        if(ch2 == 0) {
            if(dup2(p2[0], 0) == -1) {
                perror("Cannot connect ch2\'s stdin");
            }
            if(dup2(p1[1], 1) == -1) {
                perror("Cannot connect ch2\'s stdout");
            }

            execl("./dec.out", "./dec.out", NULL);

            perror("Exec failed");
            return 1;
        }
        // parent: waits for its children's termination
        else {
            if(waitpid(-1, NULL, 0) == -1) {
                perror("Failed waitpid()");
                return 2;
            }
        }
    }
    else {
        printf("Usage: %s <n>\n", argv[0]);
    }
    return 0;
}
