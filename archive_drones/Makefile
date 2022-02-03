CC = gcc
LIBFLAG = -lm -lrt -pthread

src/drone1: src/drone1_mvt.c
	$(CC)  src/drone1_mvt.c -o bin/drone1 $(LIBFLAG)
	
src/master: src/master.c 
	$(CC)  src/master.c -o bin/master $(LIBFLAG)
	
clean:
	rm -f bin/master bin/drone1 
