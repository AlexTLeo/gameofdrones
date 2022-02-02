gcc src/master.c -o bin/master -lrt -pthread -lm &>> logs/errors.log
#gcc src/drone0.c -o bin/drone0 -lrt -pthread -lm &>> logs/errors.log
#gcc src/drone1.c -o bin/drone1 -lrt -pthread -lm &>> logs/errors.log
#gcc src/drone2.c -o bin/drone2 -lrt -pthread -lm &>> logs/errors.log
#gcc src/drone3.c -o bin/drone3 -lrt -pthread -lm &>> logs/errors.log
gcc src/drone4.c -o bin/drone4 -lrt -pthread -lm &>> logs/errors.log

gnome-terminal -- sh -c "./bin/master; bash"
#sleep 1
gnome-terminal -- sh -c "./bin/drone4 4000 2 2;"
#sleep 1
gnome-terminal -- sh -c "./bin/drone4 4001 10 10;"
#sleep 1
gnome-terminal -- sh -c "./bin/drone4 4002 30 30;"
#sleep 1
gnome-terminal -- sh -c "./bin/drone4 4003 70 3;"
#sleep 1
gnome-terminal -- sh -c "./bin/drone4 4004 43 12;"