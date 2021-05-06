# TODO: finish this

!#/bin/bash

read -p "#Clients: " n
read -p "#Requests per client: " rpc
# start the server first
echo "Server starts"
./server.out
# create n client processes and
for i in {i..$n}; do
	./client &

