#!/bin/sh

#Using the find command (man 1 find) print a list of all files in your home dir
#fulfilling these conditions:
#	1) have been modified in the last X minutes (option mmin) \
#	2) contain in them the word 'Y'
#Remarks:	to make find select only regular files the option to pass is -type f
#		the -l option in grep prints the filename of the file whose content matched
#		the pattern
find ~ -type f -mmin -10 -exec grep -l 'sh' {};
