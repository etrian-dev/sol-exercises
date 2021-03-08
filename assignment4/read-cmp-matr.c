// reads the matrices in matr_dump.txt and matr_dup.dat and compares their contents
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// two useful macros
#define OK "EQUAL"
#define NOT_OK "DIFFERENT"

//	isNumber ritorna
//	0: ok
//	1: non e' un numbero
//	2: overflow/underflow
int isNumber(const char *s, long *n);
// The compare function takes a function pointer to compare the memory
// pointers to two matrices and their size (square matrices)
const char *compare(
	int (*cmpfun)(const void *, const void *, size_t),
	float *m1,
	float *m2,
	size_t size);

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("Usage: %s <\"file.txt\"> <\"file.dat\"> <dimension>\n", argv[0]);
		return EXIT_FAILURE;
	}
	// store the dimension of the matrices
	long int n = 0;
	if (isNumber(argv[3], &n) != 0)
	{
		printf("Sorry, %s is not a valid dimension", argv[3]);
		exit(EXIT_FAILURE);
	}
	// Open the files passed as arguments
	// The first is the text file (.txt) and the second the binary file (.dat)
	FILE *in_ascii = fopen(argv[1], "r");
	FILE *in_bin = fopen(argv[2], "rb");
	if (!in_ascii)
	{
		perror(argv[1]);
		exit(EXIT_FAILURE);
	}
	if (!in_bin)
	{
		perror(argv[2]);
		exit(EXIT_FAILURE);
	}
	// alloc two matrices to store the files'contents
	float *m1 = calloc(n * n, sizeof(float));
	float *m2 = calloc(n * n, sizeof(float));
	if (!(m1 && m2))
	{
		perror("Alloc of m1 or m2 failed");
		// close the files before exiting
		fclose(in_ascii);
		fclose(in_bin);
		exit(EXIT_FAILURE);
	}
	// read the matrices from the files
	size_t i, j;
	int rval;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			// read from the text file with a scanf
			rval = fscanf(in_ascii, " %f", m1 + i * n + j);
			if (rval == -1 || rval != 1)
			{
				perror("ascii file read failed");
				exit(EXIT_FAILURE);
			}
			// read from the binary file with a fread
			rval = fread(m2 + i * n + j, sizeof(float), 1, in_bin);

// debug print
#if defined(DEBUG)
			printf("read %f into m1[%lu][%lu]\nread %f into m2[%lu][%lu]\n", *(m1 + i * n + j), i, j, *(m2 + i * n + j), i, j);
#endif
		}
	}
	// close files (they're not needed anymore)
	fclose(in_ascii);
	fclose(in_bin);

	// now the matrices need to be compared to ensure they're the same
	// I will be using the function memcmp to do this
	puts(compare(memcmp, m1, m2, n));

	// free mem
	free(m1);
	free(m2);
	return 0;
}

//	isNumber ritorna
//	0: ok
//	1: non e' un numbero
//	2: overflow/underflow
int isNumber(const char *s, long *n)
{
	if (s == NULL)
		return 1;
	if (strlen(s) == 0)
		return 1;
	char *e = NULL;
	errno = 0;
	long val = strtol(s, &e, 10);
	if (errno == ERANGE)
		return 2; // overflow
	if (e != NULL && *e == (char)0)
	{
		*n = val;
		return 0; // successo
	}
	return 1; // non e' un numero
}

const char *compare(
	int (*cmpfun)(const void *, const void *, size_t),
	float *m1,
	float *m2,
	size_t size)
{
	if (cmpfun((void *)m1, (void *)m2, sizeof(float) * size) == 0)
	{
		return OK;
	}
	else
	{
		return NOT_OK;
	}
}