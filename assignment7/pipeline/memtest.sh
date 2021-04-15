#!/bin/bash

# testing script for the pipeline tokenizer
# $1 is the program to be tested
for i in testcases/*; do
	echo -n "processing $i";
	if valgrind -q --show-leak-kinds=none \
				--exit-on-first-error=yes --error-exitcode=255 \
				--leak-check=full $1 $i > /dev/null
	then echo " => OK";
	else echo " => Memory leak!";
	fi
done
