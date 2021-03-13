/* A square matrix of floats, whose dimension is specified as the only argument,
 * is alloc'd, initialized and written both to a binary file and text file
 * The binary output file is mat_dump.dat
 * The text output file is mat_dump.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>

// defines a maximum value for the matrix dimension
#define MAX_N 256
// defines the files to be written
#define BIN_FILE "mat_dump.dat"
#define TEXT_FILE "mat_dump.txt"

//	isNumber ritorna
//	0: ok
//	1: non e' un numbero
//	2: overflow/underflow
int isNumber(const char *s, long *n);

// The program receives as a parameter the size of the matrix
// and dumps a matrix to the files specified above
int main(int argc, char **argv)
{
  // not the right amount of args => usage message
  if (argc != 2)
  {
    printf("Usage: %s <mat dimension>\n", argv[0]);
    return 1;
  }
  // check if the size is actually a positive integer (less than MAX_N)
  long int n = 0;
  if (isNumber(argv[1], &n) != 0 || n > MAX_N || n <= 0)
  {
    printf("The parameter n must be an integer in the range [1,%u]\n", MAX_N);
    exit(EXIT_FAILURE);
  }

  // alloc a square matrix of floats in a (contiguous, by row)
  float *mat = calloc(n * n, sizeof(float));
  if (!mat)
  {
    perror("Cannot alloc matrix");
    exit(EXIT_FAILURE);
  }
  // initialize the matrix, stored by row
  int i, j;
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      *(mat + i * n + j) = (i + j) / 2.0;
    }
  }
  // open output files
  FILE *bin_out, *ascii_out;
  bin_out = fopen(BIN_FILE, "wb");
  ascii_out = fopen(TEXT_FILE, "w");
  if (!bin_out)
  {
    perror(BIN_FILE);
    exit(EXIT_FAILURE);
  }
  if (!ascii_out)
  {
    perror(TEXT_FILE);
    exit(EXIT_FAILURE);
  }

  // write data on both
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      fprintf(ascii_out, "%f\n", *(mat + i * n + j));
      fwrite(mat + i * n + j, sizeof(float), 1, bin_out);
      
      // debug macro
	  #if defined(DEBUG)
      printf("mat[%d][%d] = %f\n", i, j, *(mat + i * n + j));
      #endif
    }
  }

  // now close the files and free memory
  fclose(bin_out);
  fclose(ascii_out);
  free(mat);

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
