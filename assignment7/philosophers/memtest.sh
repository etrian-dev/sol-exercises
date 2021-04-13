#!/bin/bash

# testing script for the dining philosopher problem
# reads the parameters and executes the program, whose output is sent to stdout
read -p "#philosophers: " philosophers
read -p "#dinners: " dinners
valgrind -q --show-leak-kinds=none \
		--exit-on-first-error=yes --error-exitcode=255 \
		--leak-check=full \
		$1 $philosophers $dinners
