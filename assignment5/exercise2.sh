#!/bin/sh
cut -d ':' -f 1,6 /etc/passwd | grep '/home/' | sort | cut -d ':' --output-delimiter=" " -f 1,2
