#!/bin/bash

# testing script for the pipeline tokenizer
# $1 is the program to be tested
$1 testcases/input0.txt > my-output.txt
diff testcases/output0.txt my-output.txt
