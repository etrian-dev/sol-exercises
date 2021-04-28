
/*
 * Realizzare una semplice calcolatrice scientifica testuale.
 * Il programma legge da tastiera una operazione alla volta, una per riga
 * (es. sqrt(3) + 2/5), quindi per ogni richiesta forka ed esegue la calcolatrice
 * testuale fornita dal sistema bc (tipicamente installato in /usr/bin) a cui,
 * tramite pipe senza nome, passa l'operazione da svolgere e prende il risultato
 * calcolato. Il risultato ottenuto verrà stampato sullo standard output
 * secondo il seguente formato:
 *
 * Operazione: op
 * Risultato : res
 */

// sycall headers
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// std headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE_LEN 100

// wires pipes so that stdin(0) points to pipe_read and pipe_write points to stdout(1)
int redirection(int pipe_read, int pipe_write);
// sends the line ln of size sz on the pipe's write descriptor
int send_line(char *ln, size_t sz, int pipe_write);
// reads from pipe_read into res (changes pointer)
int receive_res(char **res, int pipe_read);
// displays output in the following format:
// Operazione: %s\n
// Risultato: %s\n (ricevuto da bc(1))
void display_output(char *line, char *res);

int main(int argc, char **argv) {
	// reads a line from stdin and processes it until "quit" is supplied, then terminates
	char *line = calloc(MAX_LINE_LEN, sizeof(char));
	if(!line) {
		perror("input line cannot be alloc'd");
		return 1;
	}

	int err = 0; // any error that might occurr triggers loop termination

	// read a line
        printf(">> ");
	if(fgets(line, MAX_LINE_LEN, stdin) == NULL) {
		fprintf(stderr, "line read failed\n");
		err = 1;
	}
	while(!err && strncmp(line, "quit\n", MAX_LINE_LEN) != 0) {
                // for each line create pipes for communication, then fork bc

                // pipe used to send the line to the child process
                int msgin[2];
                if(pipe(msgin) == -1) {
                        perror("Anonymous pipe can't be created");
                        err = 1;
                        continue;
                }

                // pipe used to send the result from the child to the parent
                int resultout[2];
                if(pipe(resultout) == -1) {
                        perror("Anonymous pipe can't be created");
                        err = 1;
                        continue;
                }

		// fork a process to invoke bc(1)
		pid_t child;
		switch(child = fork()) {
                        // fork error
                        case -1: {
                                perror("Cannot fork");
                                err = 1;
                                continue;
                        }
                        // child code
                        case 0: {
                                // close input pipe write fd
                                close(msgin[1]);
                                // close output pipe read fd
                                close(resultout[0]);

                                // redirect msgin[0] to stdin and resultout[1] to stdout
                                redirection(msgin[0], resultout[1]);

                                // exec CALCULATOR = "/usr/bin/bc"
                                execlp("bc", "bc", "-q", "-l", NULL);

                                perror("cannot exec!");
                                err = 1;
                                continue;
                        }
                        // parent code
                        default: {
                                // close input pipe read fd in the parent
                                close(msgin[0]);
                                // close output pipe write fd in the parent
                                close(resultout[1]);

                                // send line to the child process trough the pipe
                                // NOTE: the string terminator is not sent because bc
                                // does not recognize it and throws an error message
                                if(send_line(line, strlen(line), msgin[1]) == -1) {
                                        err = 1;
                                        continue;
                                }

                                // receive result (alloc'd internally) from the pipe
                                char *result = NULL;
                                if(receive_res(&result, resultout[0]) == -1) {
                                        err = 1;
                                        free(result);
                                        continue;
                                }

                                // display output to the user
                                display_output(line, result);

                                // then send the message "quit\n" to quit bc
                                /*if(send_line("quit\n", 6, msgin[1]) == -1) {
                                        err = 1;
                                        continue;
                                }*/


                                // communication terminated: close all remanining descriptors
                                close(msgin[1]);
                                close(resultout[0]);
                                // wait the child's termination
                                waitpid(child, NULL, 0);

                                free(result);
                        }
                }
		// read a new line
                printf(">> ");
		if(fgets(line, MAX_LINE_LEN, stdin) == NULL) {
			fprintf(stderr, "line read failed\n");
			err = 1;
		}
	}

	free(line);
	return 0;
}

int redirection(int pipe_read, int pipe_write) {
	// duplicate pipe_read into stdin (0), so that everything written into the pipe
        // gets sent to this process's stdin
        if(dup2(pipe_read, 0) == -1) {
                printf("%d: Cannot dup2(%d, 0): %s\n", getpid(), pipe_read, strerror(errno));
                return -1;
        }
        // pipe_read is now useless because it's duplicated
        close(pipe_read);

        // duplicate the pipe write fd so that the child's stdout (1) is redirected to the pipe
        if(dup2(pipe_write, 1) == -1) {
                printf("%d: Cannot dup2(%d, 1): %s\n", getpid(), pipe_write, strerror(errno));
                return -1;
        }
        // redirect stderr to pipe_write as well
	if(dup2(pipe_write, 2) == -1) {
		printf("%d: Cannot dup2(%d, 2): %s\n", getpid(), pipe_write, strerror(errno));
                return -1;
	}

	return 0;
}

int send_line(char *ln, size_t mlen, int pipe_write) {
	int ret;
	// then send the line on the pipe
	if((ret = write(pipe_write, ln, mlen)) == -1) {
		fprintf(stderr, "Message write to %d failed: %s\n", pipe_write, strerror(ret));
		return -1;
	}

        // all went well
	return 0;
}

int receive_res(char **res, int pipe_read) {
        char *rbuf = calloc(MAX_LINE_LEN, sizeof(char));
        char *tok_state = NULL;
        if(!rbuf) {
                perror("Cannot alloc result buffer");
        }
        int ret;
        switch(ret = read(pipe_read, rbuf, MAX_LINE_LEN)) {
                case -1: // read error: perror and return error to the caller
                        perror("Cannot read from pipe");
                        return -1;
                case 0: // all write descriptors on pipe have been closed
                        fprintf(stderr, "All write descriptors on pipe are closed\n");
                        return -1;
                default:
                        *res = strtok_r(rbuf, "\n", &tok_state);
        }
        return 0;
}

void display_output(char *line, char *res) {
        // display the line: done here to avoid garbling line
        char *strtok_state = NULL;
        char *stripped_newline = strtok_r(line, "\n", &strtok_state);
        printf("Operation: %s\n", stripped_newline);
        printf("Risultato: %s\n", res);
}
