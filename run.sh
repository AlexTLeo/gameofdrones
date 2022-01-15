#!/bin/bash
gcc src/master.c -o bin/master -lrt -pthread -lm &>> logs/errors.log
gcc src/clientTest.c -o bin/clientTest -lrt -pthread -lm &>> logs/errors.log

gnome-terminal -- sh -c "./bin/master; bash"
gnome-terminal -- sh -c "./bin/clientTest 4000;"
gnome-terminal -- sh -c "./bin/clientTest 4001;"
gnome-terminal -- sh -c "./bin/clientTest 4002;"
gnome-terminal -- sh -c "./bin/clientTest 4003;"
gnome-terminal -- sh -c "./bin/clientTest 4004;"
