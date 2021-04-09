/*
    Realizzare una shell rudimentale (dummyshell) che legge un comando con 
    eventuali parametri dallo standard input e ne invoca l'esecuzione utilizzando 
    una funzione di libreria della famiglia exec*. La shell deve terminare se 
    viene digitato il comando 'exit'. Il formato dei comandi accettati dalla 
    shell e' molto semplice e non non prevede metacaratteri, redirezione, pipe, 
    lettura di variabili d'ambiente, etc… 
*/

// syscall headers
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
// std headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

// declarations of functions not part of c99
char *strtok_r(char *str, const char *delim, char **saveptr);
char *strndup(const char *s, size_t n);

#define BUFSZ 512 // maximum command lenght (actually 510, but 2 are needed for \n\0)

// Welcome, Goodbye and prompt macros
#define WELCOME(sh) "Welcome to %s\n", (sh)
#define GOODBYE(sh) "Bye. Thanks for choosing %s\n", (sh)
#define PROMPT ">> "
// The exit command is the following string
#define EXIT_CMD "exit"
#define EXIT_CMD_LEN 4
// Adds an argument to the array pointed to by argv at position len
#define ADD_ARG(argv, len)                           \
    if (*(argv))                                     \
    {                                                \
        *(*(argv) + (len)) = malloc(sizeof(char *)); \
        if (*(*(argv) + (len)) == NULL)              \
        {                                            \
            perror("Cannot alloc new argv pointer"); \
            exit(EXIT_FAILURE);                      \
        }                                            \
    }                                                \
    else                                             \
    {                                                \
        *(argv) = malloc(sizeof(char *));            \
        if (!(*(argv)))                              \
        {                                            \
            perror("Cannot alloc new argv array");   \
            exit(EXIT_FAILURE);                      \
        }                                            \
    }
// cleanup function: frees the vector of lenght items
void cleanup(char **arg_vector, size_t items);

int main(int argc, char **argv)
{
    // print welcome message
    printf(WELCOME(argv[0]));

    char *buf = calloc(BUFSZ, sizeof(char));
    if (!buf)
    {
        perror("Cannot alloc buffer");
        return EXIT_FAILURE;
    }

    size_t n_tokens = 0;
    char *strtok_state = NULL;
    char *next_tok = NULL;
    char **shell_args = NULL;
    int child_pid;

    // print the prompt before entering the loop
    printf(PROMPT);
    if (fflush(stdout) == -1)
    {
        perror("Failed to flush stdout: output may be garbled");
    }
    // read a line from stdin in a buffer, then tokenize it
    while (fgets(buf, BUFSZ, stdin) != NULL && strncmp(buf, EXIT_CMD, EXIT_CMD_LEN) != 0)
    {
		shell_args = malloc(sizeof(char*));
		if(shell_args == NULL) {
			perror("Cannot alloc shell args");
			continue;
		}
		
        buf[strlen(buf) - 1] = '\0';
        // parse the string to obtain the command and its args
        // This rudimental shell assumes that the command to be executed is always
        // the first token processed
        n_tokens = 0;
        strtok_state = NULL;
        next_tok = strtok_r(buf, " ", &strtok_state);
        while (next_tok)
        {
            // one more token found in buf
            ++n_tokens;
            // add another entry in the argument array
            //ADD_ARG(&shell_args, n_tokens);
            shell_args = realloc(shell_args, n_tokens * sizeof(char*));
            // make a duplicate of the token and store it in the new array cell
            shell_args[n_tokens - 1] = strndup(next_tok, strlen(next_tok));
            if (shell_args[n_tokens - 1] == NULL)
            {
                perror("Token cannot be copied");
                cleanup(shell_args, n_tokens);
                exit(EXIT_FAILURE);
            }

            // get the next token in buf
            next_tok = strtok_r(NULL, " ", &strtok_state);
        }
        // a new NULL pointer must be added at the end of shell_args to conform
        // argv syntax
        shell_args = realloc(shell_args, (n_tokens + 1) * sizeof(char*));
        shell_args[n_tokens] = NULL;

#ifdef DEBUG // prints all the tokens to be passed to execv
		int i;
        for (i = 0; i < n_tokens; i++)
        {
            printf("argv[%d] = %s\n", i, shell_args[i]);
        }
        printf("argv[%d] = NULL\n", i);
#endif

        // fork() and then execv() to execute the command
        if ((child_pid = fork()) == -1)
        {
            perror("Cannot fork()");
            cleanup(shell_args, n_tokens);
            
        }

        // Code executed by the child: calls exec to launch the command
        // execvp is used to automatically search for executables in PATH
        if (child_pid == 0)
        {
            // all the argument s (if any) are stored in char **shell_args
            execvp(shell_args[0], shell_args);
            // if it returned, it's an error for sure
            perror("Cannot exec the program");
            cleanup(shell_args, n_tokens);
            exit(-1);
        }
        // Code executed by the parent: wait for the newly created child, and then do stuff
        else
        {
            int ret;
            if (waitpid(child_pid, &ret, 0) == -1)
            {
                perror("waitpid() failed");
                exit(EXIT_FAILURE);
            }
            // if the child didn't exit normally, print an error message to the user
            if (WIFEXITED(ret))
            {
                printf("child %d terminated with status %d\n", child_pid, WEXITSTATUS(ret));
            }
            else if (WIFSIGNALED(ret)) {
				printf("child %d terminated by signal %d\n", child_pid, WTERMSIG(ret));

			}
			else if (WIFSTOPPED(ret)) {
				printf("child %d stopped by signal %d\n", child_pid, WSTOPSIG(ret));
			}
            
            cleanup(shell_args, n_tokens);
        }

        // reset buffer
        buf = memset(buf, (char)0, BUFSZ * sizeof(char));
        // Then print the prompt to receive the next command
        printf(PROMPT);
        if (fflush(stdout) == -1)
        {
            perror("Failed to flush stdout: output may be garbled");
        }
    }
    //  clean up the data structure used to store args
	//cleanup(shell_args, n_tokens);
    free(buf);

    printf(GOODBYE(argv[0]));

    return EXIT_SUCCESS;
}

void cleanup(char **arg_vector, size_t items)
{
    size_t i;
    for (i = 0; i < items; i++)
    {
        if (arg_vector[i])
        {
            free(arg_vector[i]);
        }
    }
    free(arg_vector);
}
