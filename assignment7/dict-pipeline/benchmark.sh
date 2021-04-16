#!/bin/bash

printf "%-25s\t%s\n" "FILE" "TIME AND RESOURCES"
for i in testcases/*; do
	printf "%-25s\t" $i;
	time -f "user: %U kernel: %S Elapsed: %E CPU: %P" $1 $i > /dev/null;
done