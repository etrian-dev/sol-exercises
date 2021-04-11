#!/bin/bash

# testing script for the pipeline tokenizer
# $1 is the program to be tested
for i in testcases/*; do
	echo -n "processing $i";
	valgrind -q $1 $i | diff -w - $i;
	# the -w flag ignores whitespace
	echo " => OK";
done