#!/bin/bash

gcc src/drone3.c -o bin/drone3 -lrt -pthread -lm & >> logs/errors.log



gnome-terminal -- sh -c "./bin/master; bash" &
sleep 1

gnome-terminal -- sh -c "./bin/drone0; bash"
gnome-terminal -- sh -c "./bin/drone1; bash"
gnome-terminal -- sh -c "./bin/drone2; bash"
gnome-terminal -- sh -c "./bin/drone3; bash"
gnome-terminal -- sh -c "./bin/drone4; bash"


