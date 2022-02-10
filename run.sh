#!/bin/bash
gcc src/master.c -o bin/master -lrt -pthread -lm &>> logs/errors.log
gcc src/drone4.c -o bin/drone4 -lrt -pthread -lm &>> logs/errors.log

gnome-terminal -- sh -c "./bin/master; bash"
gnome-terminal -- sh -c "./bin/drone0"
gnome-terminal -- sh -c "./bin/drone1"
gnome-terminal -- sh -c "./bin/drone2"
gnome-terminal -- sh -c "./bin/drone3"
gnome-terminal -- sh -c "./bin/drone4"