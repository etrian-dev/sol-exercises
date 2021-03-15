#!/bin/bash

# This simple script tests the myfind executable, passed in as $1

# read the directory to search from and then the pattern to be searched
read -r -e -p "Source dir: " source
read -r -e -p "Pattern: " pat
$1 $source $pat # pattern is quoted just to be safe
# outputs various information about files matching pat (descends recursively in subdirs)
