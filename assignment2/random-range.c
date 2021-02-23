// generates n random numbers within a range [k1, k2[
// and prints their relative frequencies
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

// define default values for n, k1 and k2
#define N 100000
#define K1 100
#define K2 120

void print_freq(const int *freq, const long int a, const long int b, const float tries) {
  puts("Occorrenze di:");
  int i = a;
  while(i < b) {
    printf("%d: %2.2f%%\n", i, (float)(100 * freq[i-a])/tries);
    i++;
  }
}

#define BASE 10
long int longint_of_str(char *str) {
  char *ptr = NULL;
  long int res = strtol(str, &ptr, BASE);
  if(errno == ERANGE && res == LONG_MIN) {
    puts("Underflow occurred");
    exit(EXIT_FAILURE);
  }
  else if(errno == ERANGE && res == LONG_MAX) {
    puts("Overflow occurred");
    exit(EXIT_FAILURE);
  }
  if(*str != '\0' && *ptr == '\0') {
    return res;
  }
  puts("String cannot be entirely converted");
  exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
  long int n;
  long int k1;
  long int k2;
  // if no arguments were given, assign default values to parameters n, k1, k2
  if(argc != 4) {
    n = N;
    k1 = K1;
    k2 = K2;
  }
  // otherwise use the ones supplied
  else {
    // n must be positive, so if n is negative falls back to N
    n = longint_of_str(argv[1]);
    if(n <= 0) {
      printf("n must be positive. Set to default value n = %d\n", N);
      n = N;
    }
    k1 = longint_of_str(argv[2]);
    k2 = longint_of_str(argv[3]);
  }
  // if the boundaries are swapped (safety net)
  if(k1 > k2) {
    long int tmp = k1;
    k1 = k2;
    k2 = tmp;
  }
  else if(k1 == k2) {
    puts("Empty range!");
    return EXIT_FAILURE;
  }

  // alloc memory to store values'frequencies
  int *val_frequencies = calloc(k2 - k1 + 1, sizeof(int));
  if(val_frequencies == NULL && errno == ENOMEM) {
    char *err = strerror(errno);
    perror(err);
    free(err);
    return EXIT_FAILURE;
  }
  // generate (pseudo)random values with rand_r()
  int i;
  unsigned int seed = 10;
  for(i = 0; i < n; i++) {
    int val = k1 + (rand_r(&seed) % (k2 - k1 + 1));
    val_frequencies[val - k1] += 1;
  }
  // then print the frequency table
  print_freq(val_frequencies, k1, k2, (float)n);

  // free memory
  free(val_frequencies);

  return EXIT_SUCCESS;
}
