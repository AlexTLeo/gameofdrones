#!/bin/bash
gcc src/master.c -o bin/master -lrt -pthread -lm &>> logs/errors.log &
gcc src/drone4.c -o bin/drone4 -lrt -pthread -lm &
gcc src/clientTest.c -o bin/clientTest -lrt -pthread -lm &
gnome-terminal -- sh -c "./bin/master; bash" &
sleep 1 
gnome-terminal -- sh -c "./bin/drone4; bash"