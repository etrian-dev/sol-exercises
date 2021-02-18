#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int REALLOC_INC=16;

char *mystrcat(char* buf, size_t sz, char *first, ...);
 
void RIALLOCA(char** buf, size_t newsize) {
  char *newbuf = realloc(*buf, newsize);
  if(!newbuf) {
  	puts("Memory cannot be alloc'd");
  	exit(EXIT_FAILURE);
  }
  *buf = newbuf;
}     
char* mystrcat(char *buf, size_t sz, char *first, ...) {
	va_list strlist;
	va_start(strlist, first); // parte dalla stringa first
	
	size_t tot_sz = sz;
	size_t actual_len = strlen(buf);
	
	while(tot_sz - actual_len < strlen(first) + 1) {
		// rialloco il buffer
		RIALLOCA(&buf, tot_sz + strlen(first) + 1);
		tot_sz = tot_sz + REALLOC_INC; // aggiorno la size totale del buffer
	}
	actual_len = strlen(first);
	strncat(buf, first, strlen(first) + 1);
	
	char *elem = va_arg(strlist, char*);
	while(elem != NULL) { // la lista di argomenti è terminata da NULL
		while(tot_sz - actual_len < strlen(elem) + 1) {
			// rialloco il buffer
			RIALLOCA(&buf, tot_sz + strlen(elem) + 1);
			tot_sz = tot_sz + REALLOC_INC; // aggiorno la size totale del buffer
		}
		actual_len += strlen(elem);
		// concateno la stringa
		strncat(buf, elem, strlen(elem) + 1);
		// prossimo argomento
		elem = va_arg(strlist, char*);
	}
	// cleanup
	va_end(strlist);
	// ritorno la stringa
	return buf;
}  
 
int main(int argc, char *argv[]) {
  if (argc != 7) {
  	printf("troppi pochi argomenti\n");
  	return -1;
  }
  char *buffer=NULL;
  RIALLOCA(&buffer, REALLOC_INC);  // macro che effettua l'allocazione del 'buffer'
  buffer[0]='\0'; // mi assicuro che il buffer contenga una stringa
  buffer = mystrcat(buffer, REALLOC_INC, argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], NULL);
  printf("%s\n", buffer);
  // NOTA: se avessi scritto printf("%s\n", mystrcat(....)); avrei perso il puntatore
  // al buffer restituito da mystrcat; di conseguenza avrei avuto un memory leak
  free(buffer);
  return 0;
}
