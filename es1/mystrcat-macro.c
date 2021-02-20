#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// realloc size expansion
const int REALLOC_INC = 16;

// macro to perform reallocation
#define RIALLOCA(buf, newsize)                 \
	char *newbuf = realloc(*(buf), (newsize)); \
	if (!newbuf)                               \
	{                                          \
		puts("Memory cannot be alloc'd");      \
		exit(EXIT_FAILURE);                    \
	}                                          \
	*(buf) = newbuf;

// concatenates the strings passed as arguments in a buffer buf
// returns the final result as a pointer
char *mystrcat(char *buf, size_t sz, char *first, ...)
{
	// the variable argument list, starting after first is initialized
	va_list strlist;
	va_start(strlist, first);
	// tracks buf's total allocated size (in bytes) and the lenght of the stored string
	size_t alloc_sz = sz;
	size_t buf_len = strlen(buf);
	// stores the argument's size to avoid computing it every iteration
	size_t arg_sz = strlen(first) + 1;
	// while there's not enough memory for char *first alloc'd, realloc buf
	while (alloc_sz - buf_len < arg_sz)
	{
		RIALLOCA(&buf, alloc_sz + REALLOC_INC);
		alloc_sz += REALLOC_INC;
	}
	// the buffer now contains the string first, but its size could be greater
	buf_len = arg_sz - 1;
	strncat(buf, first, buf_len);

	// for each element in the list, do the same thing
	char *elem = va_arg(strlist, char *);
	while (elem != NULL) // the argument list is terminated by NULL
	{
		arg_sz = strlen(elem) + 1; // save arg's size
		while (alloc_sz - buf_len < arg_sz)
		{
			RIALLOCA(&buf, alloc_sz + REALLOC_INC);
			alloc_sz += REALLOC_INC;
		}
		buf_len += arg_sz - 1;
		// then concatenate the string
		strncat(buf, elem, buf_len);
		// then get the next argument in the list
		elem = va_arg(strlist, char *);
	}
	va_end(strlist);

	// returns the string
	return buf;
}

// The main function takes 6 arguments and concatenates them,
// then prints the result to stdout
int main(int argc, char *argv[])
{
	if (argc != 7)
	{
		printf("Too few arguments\n");
		return -1;
	}
	char *buffer = NULL;
	RIALLOCA(&buffer, REALLOC_INC); // macro che effettua l'allocazione del 'buffer'
	buffer[0] = '\0';				// mi assicuro che il buffer contenga una stringa
	buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
	printf("%s\n", buffer);
	// NOTA: se avessi scritto printf("%s\n", mystrcat(....)); avrei perso il puntatore
	// al buffer restituito da mystrcat; di conseguenza avrei avuto un memory leak
	free(buffer);
	return 0;
}
