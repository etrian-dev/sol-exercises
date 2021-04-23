# Esercizi SOL
Una serie di programmi realizzati principalmente in linguaggio C durante il corso Sistemi Operativi e Laboratorio
tenuto durante l'A.A. 2020-2021
## Elenco
* [variadic arguments](./varargs)
  * sum of integers
* [first assignment: strings, parsing and allocation](./assignment1)
  * strtoupper: translates ascii string to uppercase
  * mystrcat: implements strcat on 6 strings
  * getoptlike: reads and prints some command line options (like getopt)
* [second assignment: option parsing and various](./assigment2)
  * options parsing with getopt (extends #3 of the previous assignment)
  * semantically the same as the previous, but with function pointers
  * reentrant tokenizer on 2 strings, using strtok_r
  * generation of random numbers within a range + statistics
* [third assignment: linking and reentrancy](./assignment3)
  * static/dynamic libraries (libtokenizer)
  * macros for matrices
  * suffix sum (both non-reentrant and reentrant)
  * use of extern to define things without including headers
* [fourth assignment: file I/O](./assignment4)
  * lists usernames in /etc/passwd
  * writes a matrix to text and binary files. Another program compares the results
  * A simplifed version of the wc command (options -w and -l)
* [fifth assignment: syscalls vs libc calls](./assignment5)
  * a clone of the cp command. One version uses syscalls and the other libc calls
  * a very simple clone of the find command, using libc calls for directories
  * a very simple clone of the ls command, using libc calls for directories
* [sixth assignment: fork() and exec*()](./assignment6)
  * dummysh: a simple shell clone using fork/exec syscalls
  * a program that sleeps for a certain amount of seconds, then prints some PIDs
  * a program that creates a certain number N of zombie processes
  * a program that generates a process chain of lenght N, printed on stdout
  * a program that calculates the n-th term (n<=13) of the fibonacci sequence by forking processes
* [seventh assignment: multithreading](./assignment7)
  * Producer/Consumer problem, using a shared buffer of size 1
  * Classical Dining philosophers problem, solved with three different approaches:
    1. ordering in resource acquisition
    1. try-lock and busy wait
    1. monitors of the neighobours'states. Neighbours are helped if philosopher i finishes eating
  * Concurrent pipeline with three threads: a line reader, a tokenizer and a printer. They interact through a FIFO queue
  * Extends the program above by having the third thread print each token exactly once, despite the number of occurrences. This is achieved by using an hash table as a dictionary and dumping its contents when all tokens have been received
* [eight assignment: pipes & threads](./assignment8)
  * Multithreaded producer/consumer: n producers and m consumers with a FIFO queue (unbounded) of k messages
  * A program that evaluates an expression using bc(1) connected troough an anonymous pipe
  * given the program produced by [dec.c](./assignment8/pipedec/dec.c) forks two copies and creates a bidirectional connection trough pipes
