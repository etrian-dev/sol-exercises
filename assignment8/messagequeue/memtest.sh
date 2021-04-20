#!/bin/bash

read -p "#messages: " mess
read -p "#producers: " prod
read -p "#consumers: " cons

# testing script for the message queue
# $1 is the program to be tested
if valgrind \
	--show-leak-kinds=all \
	--exit-on-first-error=yes --error-exitcode=255 \
	--leak-check=full $1 $mess $prod $cons; \
	then
		echo " => OK";
else echo " => Memory leak!";
fi

