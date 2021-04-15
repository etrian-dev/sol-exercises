#!/bin/bash

# testing script for the dining philosopher problem
# reads the parameters and executes the program, whose output is sent to stdout
echo "Memory leak test on solution $1"
read -p "#philosophers: " philosophers
read -p "#dinners: " dinners

valgrind -q --show-leak-kinds=all \
		--exit-on-first-error=yes --error-exitcode=255 \
		--leak-check=full \
		./phi$1".out" $philosophers $dinners
