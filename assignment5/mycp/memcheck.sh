#!/bin/sh
#choose the buffer size
read -p "Buffer size: " bufsz
# $1 is the executable using system calls
# $2 is the executable launched C library calls
for i in input/*; 
do x='output/'$(echo $i | cut -d '/' -f 2);
valgrind -q --log-file="valgrind-syscalls-%p.log" $1 $i $x $bufsz;
valgrind -q --log-file="valgrind-libcalls-%p.log" $2 $i $x $bufsz;
done
cat valgrind-syscalls*.log > memcheck-sys.log
cat valgrind-libcalls*.log > memcheck-libc.log
rm valgrind*.log

