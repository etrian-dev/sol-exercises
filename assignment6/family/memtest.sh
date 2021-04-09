#!/bin/bash
read -p "Number of processes? " num
valgrind --leak-check=full $1 $num
