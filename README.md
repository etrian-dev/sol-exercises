# Esercizi SOL
Una serie di programmi realizzati principalmente in linguaggio C durante il corso Sistemi Operativi e Laboratorio
tenuto durante l'A.A. 2020-2021
## Elenco
* [variadic arguments](./varargs)
  * sum of integers
* [first assignment](./assignment1)
  * strtoupper: translates ascii string to uppercase
  * mystrcat: implements strcat on 6 strings
  * getoptlike: reads and prints some command line options (like getopt)
* [second assignment](./assigment2)
  * options parsing with getopt (extends #3 of the previous assignment)
  * semantically the same as the previous, but with function pointers
  * reentrant tokenizer on 2 strings, using strtok_r
  * generation of random numbers within a range + statistics
* [third assignment](./assignment3)
  * static/dynamic libraries (libtokenizer)
  * macros for matrices
  * suffix sum (both non-reentrant and reentrant)
  * use of extern to define things without including headers
* [fourth assignment](./assignment4)
  * lists usernames in /etc/passwd
  * writes a matrix to text and binary files. Another program compares the results
  * A simplifed version of the wc command (options -w and -l)
* [fifth assignment](./assignment5)
  * a clone of the cp command. One version uses syscalls and the other libc calls
  * a very simple clone of the find command, using libc calls for directories
  * a very simple clone of the ls command, using libc calls for directories
* [sixth assignment](./assignment6)
  * dummysh: a simple shell clone using fork/exec syscalls
  * a program that sleeps for a certain amount of seconds, then prints some PIDs
  * a program that creates a certain number N of zombie processes
  * a program that generates a process chain of lenght N, printed on stdout
  * a program that calculates the n-th term (n<=13) of the fibonacci sequence by forking processes
* [seventh assignment](./assignment7)
  * Producer/consumer threads using a shared 1-position buffer
  * Classical Dining philosophers problem, solved with three different approaches
  * Concurrent pipeline with three threads: a line reader, a tokenizer and a printer. They interact through a FIFO queue
