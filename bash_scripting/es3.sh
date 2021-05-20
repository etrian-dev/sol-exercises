#!/bin/bash

# Dati i file testuali contenuti in questo tarball, ognuno dei quali contiene per
# ogni riga due campi separati da spazio ('id valore'). Scrivere uno script bash che
# legge il campo 'valore' di ogni file e ne calcola media e deviazione standard, quindi
# stampa sullo standard output una stringa con il seguente formato:

# [nomedelfile senza estensione] [numero di linee del file] [media] [deviazione standard].
# Un esempio di output per i due file test1.dat e test2.dat è:
# test1 5 20.18 1.25
# test2 3 11.20 .81
# NOTA: per effettuare i calcoli in floating point usare il comando bc
# (esempio: echo “scale=2; sqrt(12)” | bc -q“ stampa 3.46, cioe' la radice quadrata
# di 12 con troncamento a 2 cifre dopo la virgola).

if [ -d results ]; then
    for f in results/*; do
        exec 3<$f
        i=0
        while read -u 3 ln; do
            values[$i]=$(echo -n $ln | tr -s ' ' | cut -d ' ' -f 2)
            ((i++))
        done
        expr=${values[0]}
        for ((j=1; j < $i; j++)); do
            expr=$(echo -n "$expr + ${values[$j]}")
        done
        avg=$(echo "scale=2; ($expr) / $i"| bc -q)
        # sapendo la media calcolo da dev.standard
        expr=$(echo -n "(${values[0]} - $avg)^2")
        for ((j = 0; j < $i; j++)); do
            expr=$(echo -n "$expr + (${values[$j]} - $avg)^2")
        done
        #echo $expr
        if [ $i = 1 ]; then
            # se ho un solo valore non ha senso fare sd campionaria
            sd=0
        else
            sd=$(echo "scale=2; sqrt(($expr) / ($i - 1))"| bc -q)
        fi
        echo "$(basename $f) $i $avg $sd"
    done
fi
