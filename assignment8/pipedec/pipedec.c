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

int main(int argc, char **argv) {
    if(argc == 2) {
        // pipes per comunicazione bidirezionale tra i processi ch1 e ch2
        int p1[2], p2[2];

        if(pipe(p1) == -1) {
            perror("Cannot create pipe");
            return 2;
        }
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

        // code exec by ch1: gets x
        if(ch1 == 0) {
            // redirect stdin to p1[0]
            if(dup2(p1[0], 0) == -1) {
                perror("Cannot connect ch1\'s stdin");
            }
            // redirect stdout to p2[1]
            if(dup2(p2[1], 1) == -1) {
                perror("Cannot connect ch1\'s stdout");
            }

            // all the descriptors to the pipes must be closed in the child
            // so that only 0 and 1 are open (though redirected trough pipes)
            if( close(p1[0]) == -1
                ||close(p1[1]) == -1
                || close(p2[0]) == -1
                || close(p2[1]) == -1)
            { perror("Cannot close some pipe fd"); return 2;}

            // now exec dec.out with argument x (argv[1])
            execl("./dec.out", "./dec.out", argv[1], NULL);

            perror("Exec failed");
            return 1;
        }

        if((ch2 = fork()) == -1) {
            perror("Cannot fork ch2");
            return 1;
        }

        // code exec by ch2: doesn't get x
        if(ch2 == 0) {
            // redirect stdin to p2[0]
            if(dup2(p2[0], 0) == -1) {
                perror("Cannot connect ch2\'s stdin");
            }
            // redirect stdout to p1[1]
            if(dup2(p1[1], 1) == -1) {
                perror("Cannot connect ch2\'s stdout");
            }


            // all the descriptors to the pipes must be closed in the child
            // so that only 0 and 1 are open (though redirected trough pipes)
            if( close(p1[0]) == -1
                ||close(p1[1]) == -1
                || close(p2[0]) == -1
                || close(p2[1]) == -1)
            { perror("Cannot close some pipe fd"); return 2;}

            // now exec dec.out without any argument
            execl("./dec.out", "./dec.out", NULL);

            perror("Exec failed");
            return 1;
        }

        /*
         parent code: wait for any child to terminate. Since all pipe fds
         p1[0], p1[1], ... were closed in the children as soon as one of them
         terminates because x < 0, then it closes its stdin and stdout and thus
         when the other child tries to r = read() it returns r = 0 and the process
         teminates as well (using the other exiting message: see dec.c)
         (***) Whether the parent waits for the second child's termination is irrelevant,
         because it's bound to terminate anyway and even if the parent terminates before
         its child, the zombie wolud then be terminated by init (or equivalent)
         */
        if(ch1 != 0 && ch2 != 0) {
            // The parent process needs to close its open pipe file descriptors anyway
            if( close(p1[0]) == -1
                ||close(p1[1]) == -1
                || close(p2[0]) == -1
                || close(p2[1]) == -1)
            { perror("Cannot close some pipe fd"); return 2;}

            waitpid(-1, NULL, 0); // wait for any children
            waitpid(-1, NULL, WNOHANG); // this is actually quite useless, because of (***)
        }
    }
    else {
        printf("Usage: %s <x>\n", argv[0]);
    }
    return 0;
}
