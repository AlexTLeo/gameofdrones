#!/bin/bash
gcc src/master.c -o bin/master -lrt -pthread -lm & >> logs/errors.log
gcc src/drone3.c -o bin/drone3 -lrt -pthread -lm & 
#gcc src/clientTest.c -o bin/clientTest -lrt -pthread -lm & 

gnome-terminal -- sh -c "./bin/master; bash" &
sleep 1
gnome-terminal -- sh -c "./bin/drone3; bash"
#gnome-terminal -- sh -c "./bin/clientTest 4000; bash"
#gnome-terminal -- sh -c "./bin/clientTest 4001; bash"
#gnome-terminal -- sh -c "./bin/clientTest 4002; bash"

#gnome-terminal -- sh -c "./bin/clientTest 4004; bash"
