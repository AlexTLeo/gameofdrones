#!/bin/bash
gcc src/master.c -o bin/master -lrt -pthread -lm &>> logs/errors.log
gcc src/drone0.c -o bin/drone0 -lrt -pthread -lm &>> logs/errors.log
gcc src/drone0.c -o bin/drone1 -lrt -pthread -lm &>> logs/errors.log
gcc src/drone0.c -o bin/drone2 -lrt -pthread -lm &>> logs/errors.log
gcc src/drone0.c -o bin/drone3 -lrt -pthread -lm &>> logs/errors.log
gcc src/drone0.c -o bin/drone4 -lrt -pthread -lm &>> logs/errors.log

gnome-terminal -- sh -c "./bin/master; bash"
gnome-terminal -- sh -c "./bin/drone0 4000 2 2;"
gnome-terminal -- sh -c "./bin/drone1 4001 10 10;"
gnome-terminal -- sh -c "./bin/drone2 4002 30 30;"
gnome-terminal -- sh -c "./bin/drone3 4003 70 3;"
gnome-terminal -- sh -c "./bin/drone4 4004 43 12;"
