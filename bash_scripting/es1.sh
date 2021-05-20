#!/bin/bash

if [ $# -ne 1 ]; then                        # Se il numero di argomenti è != 1 then
    echo usa: $(basename $0) nomedirectory   # Stampa messaggio: "usa <nome base script> nomedirectory"
    exit -1
fi
dir=$1
if [ ! -d $dir ]; then                       # Se dir non è una directory then
    echo "L'argomento $dir non e' una directory" # Stampa che dir (sostiuisce l'argomento) non è una directory
    exit 1;
fi

bdir=$(basename $dir)                       # Prendo solo nome directory e non il path
if [ -w $dir.tar.gz ]; then # il file esiste ed e scrivibile
    echo -n "il file $bdir.tar.gz esiste gia', sovrascriverlo (S/N)?"
    read yn                                 # Aspetta un carattere da tastiera
    if [ x$yn != x"S" ]; then               # Se legge 'S' allora esce (non sovrascrive), altrimenti sovrascrive
          exit 0;
    fi
    rm -f $bdir.tar.gz
fi
echo "creo l'archivio con nome $bdir.tar.gz"
# Crea l'archivio $bdir.tar
tar cf $bdir.tar $dir 2> error.txt          # Appende l’output sullo std-error nel file error.txt
if [ ! -e $bdir.tar ]; then                   # Controlla che il comando sia andato a buon fine
    echo "Errore nella creazione dell'archivio"
    exit 1
fi
#Ora comprimo l'archivio creato
gzip $bdir.tar 2> error.txt                 # Appende l’output sullo std-error nel file error.txt
if [ ! -e $bdir.tar.gz ]; then                # controlla che il comando sia andato a buon fine
    echo
    echo "Errore nella compressione dell'archivio"
    exit 1
fi

echo "archivio creato con successo, il contenuto dell’archivio e':"
tar tzvf $bdir.tar.gz  2>&1                 # Redirige lo std-error sullo std-output
exit 0
