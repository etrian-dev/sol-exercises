#!/bin/bash

# testing script for the dining philosopher problem
# reads the parameters and executes the program, whose output is sent to stdout
read -p "#philosophers: " philosophers
read -p "#dinners: " dinners
$1 $philosophers $dinners
