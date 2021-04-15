#!/bin/bash

# benchmarking script for the solutions to the dining philosophers
# problem. The solutions implemented are run with the same parameters
# and the resulting times (user, system, real and cpu usage) are
# printed to have a comparison, using time(1)

echo "Benchmarking solutions"
read -p "#philosophers: " philosophers
read -p "#dinners: " dinners

# executables are supplied as $1 $2 $3 respectively, as it's designed to be invoked
# by make benchmark
echo "Solution 1: ordering"
time -f "user: %U kernel: %S Elapsed: %E CPU: %P" ./$1 $philosophers $dinners > /dev/null
echo "Solution 2: busy wait"
time -f "user: %U kernel: %S Elapsed: %E CPU: %P" ./$2 $philosophers $dinners > /dev/null
echo "Solution 3: monitors"
time -f "user: %U kernel: %S Elapsed: %E CPU: %P" ./$3 $philosophers $dinners > /dev/null
