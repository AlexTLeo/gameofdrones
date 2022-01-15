#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include "../include/utils.h"
#include "../include/values.h"

/**
 * This is the master to 5 other processes called "drones". It receives their
 * packets via TCP sockets and processes them. The packest are coordinates and
 * status messages. The master will display the drones on a map and inform them
 * in case of potential collisions, amongst other things (see README).
 */

const int NUM_SOCKETS = 1;

int main (int argc, char *argv[]) {
  // Sockets
  int fdSocket[NUM_SOCKETS];
  int fdDrone[NUM_SOCKETS];
  int portno[NUM_SOCKETS], clilen;
  char buffer[256];
  struct sockaddr_in servAddr, cliAddr;
  int n;
  //  Logs
  int fdlogErr;
  int fdlogInfo;

  portno[0] = PORTNO + 1;

  fdlogErr = openErrorLog();
  fdlogInfo = openInfoLog();

  writeInfoLog(fdlogInfo, "[MASTER] Creating sockets");

  fdSocket[0] = socketCreate(AF_INET, SOCK_STREAM, 0, fdlogErr);

  // Configuring 5 sockets, loop through each socket
  // Each for loop must be separate because some are blocking calls (and for clarity) (another solution: multithreading)
  writeInfoLog(fdlogInfo, "[MASTER] Configuring sockets");

  bzero((char *) &servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind
  writeInfoLog(fdlogInfo, "[MASTER] Binding sockets");
  for (int i = 0; i < NUM_SOCKETS; i++) {
    servAddr.sin_port = htons(portno[i]);

    socketBind(fdSocket[i], (struct sockaddr*) &servAddr, sizeof(servAddr), fdlogErr);
  }

  // Listen
  writeInfoLog(fdlogInfo, "[MASTER] Listening on sockets");
  for (int i = 0; i < NUM_SOCKETS; i++) {
    socketListen(fdSocket[i], 5, fdlogErr);
  }

  // Accept
  writeInfoLog(fdlogInfo, "[MASTER] Accepting incoming connections on sockets");
  clilen = sizeof(cliAddr);
  for (int i = 0; i < NUM_SOCKETS; i++) {
    fdDrone[i] = socketAccept(fdSocket[i], (struct sockaddr *) &cliAddr, &clilen, fdlogErr);
  }

  // TODO: Change all of this!
  writeInfoLog(fdlogInfo, "[MASTER] Reading from sockets");
  for (int i = 0; i < NUM_SOCKETS; i++) {
    bzero(buffer, 256);
    n = read(fdDrone[i], buffer, 255);

    if (n < 0) {
      writeErrorLog(fdlogErr, "[MASTER] Error reading from socket", fdlogErr);
      exit(-1);
    }

    printf("[MASTER] Message received: %s\n", buffer);
    n = write(fdDrone[i], "[MASTER] Packet received", 255);

    if (n < 0) {
      writeErrorLog(fdlogErr, "[MASTER] Error writing to socket", fdlogErr);
      exit(-1);
    }
  }

  return 0;
}
