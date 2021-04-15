#!/bin/bash

# testing script for the dining philosopher problem
# reads the parameters and executes the program, whose output is sent to stdout
echo "Testing solution $1"
read -p "#philosophers: " philosophers
read -p "#dinners: " dinners

time -f "user: %U kernel: %S Elapsed: %E CPU: %P" ./phi$1".out" $philosophers $dinners
