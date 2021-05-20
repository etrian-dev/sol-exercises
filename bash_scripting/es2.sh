#!/bin/bash

# Utilizzando il comando find, stampare la lista di tutti i nomi di file contenuti
# nella propria home (o in una directory creata appositamente per fare il test) che
# sono stati modificati negli ultimi 'X' minuti e che contengono al loro interno la
# parola 'Y' (X è un numero intero Y è una stringa, es. X=2 Y=ciao). Usare il comando
# find, ricordando che: per cercare solo file regolari l'opzione e' '-type f', per
# selezionare i file modificati entro 'X' minuti l'opzione da usare è '-mmin'
# (leggere attentamente l'entry nel manuale man 1 find). Per cercare una parola
# all'interno di un file usare il comando grep, l'opzione '-l' di grep permette di
# stampare il nome del file che ha dato il match.

find $1 -type f -mmin -$2 -exec grep -l $3 '{}' \;
