#!/bin/bash

# testing script for the pipeline tokenizer
# $1 is the program to be tested
for i in testcases/*; do
	echo -n "processing $i";
	if $1 $i > /dev/null; then
		echo " => OK";
	else echo " => Failed";
	fi
done
