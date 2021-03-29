#!/bin/bash
#choose the buffer size
read -p "Buffer size: " bufsz
# $1 is the executable using system calls
# $2 is the executable launched C library calls
for i in input/*; 
do x='output/'$(echo $i | cut -d '/' -f 2);
/usr/bin/time -f "syscalls: %U user\t%S sys\t%e real\t%C" $1 $i $x $bufsz;
diff $i $x;
/usr/bin/time -f "libcalls: %U user\t%S sys\t%e real\t%C" $2 $i $x $bufsz;
diff $i $x;
echo "";
done
