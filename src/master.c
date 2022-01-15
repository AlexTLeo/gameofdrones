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

/**
 * Draws a map to terminal with drones and map borders visible
 * @param map - array of map tiles 40x80, where each value has a meaning:
 * 0 is "empty", 1 is "occupied, wall", 2 is "occupied, flying", 3 is "occupied, refueling"
 */
void drawMap(int droneMap[82][42]);

const int NUM_DRONES = 5;
const int TEXT_DELAY = 2500;
const int MAP_WIDTH = 80 + 2; // +2 for walls
const int MAP_HEIGHT = 40 + 2; // +2 for walls

int main (int argc, char *argv[]) {
  int droneMap[82][42];
  int droneCoords[5][2];
  bool isRunning = true;
  bool isOccupied = false;
  // Sockets
  int fdSocket[NUM_DRONES];
  int fdDrone[NUM_DRONES];
  int portno[NUM_DRONES], clilen;
  struct sockaddr_in servAddr, cliAddr;
  const int optVal = 1;
  const socklen_t optLen = sizeof(optVal);
  //  Logs
  int fdlogErr;
  int fdlogInfo;

  portno[0] = PORTNO;
  portno[1] = PORTNO + 1;
  portno[2] = PORTNO + 2;
  portno[3] = PORTNO + 3;
  portno[4] = PORTNO + 4;

  fdlogErr = openErrorLog();
  fdlogInfo = openInfoLog();

  writeInfoLog(fdlogInfo, "[MASTER] Creating sockets");

  // Create sockets
  for (int i = 0; i < NUM_DRONES; i++) {
    fdSocket[i] = socketCreate(AF_INET, SOCK_STREAM, 0, fdlogErr);
    socketSetOpt(fdSocket[i], SOL_SOCKET, SO_REUSEPORT, (void*) &optVal, optLen, 2);
  }

  // Configuring 5 sockets, loop through each socket
  // Each for loop must be separate because some are blocking calls (and for clarity) (another solution: multithreading)
  writeInfoLog(fdlogInfo, "[MASTER] Configuring sockets");

  bzero((char *) &servAddr, sizeof(servAddr));
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind
  writeInfoLog(fdlogInfo, "[MASTER] Binding sockets");
  for (int i = 0; i < NUM_DRONES; i++) {
    servAddr.sin_port = htons(portno[i]);

    socketBind(fdSocket[i], (struct sockaddr*) &servAddr, sizeof(servAddr), fdlogErr);
  }

  // Listen
  writeInfoLog(fdlogInfo, "[MASTER] Listening on sockets");
  for (int i = 0; i < NUM_DRONES; i++) {
    socketListen(fdSocket[i], 5, fdlogErr);
  }

  // Accept
  writeInfoLog(fdlogInfo, "[MASTER] Accepting incoming connections on sockets");
  clilen = sizeof(cliAddr);
  for (int i = 0; i < NUM_DRONES; i++) {
    fdDrone[i] = socketAccept(fdSocket[i], (struct sockaddr *) &cliAddr, &clilen, fdlogErr);
  }

  // Initially, fill map with walls
  for (int i = 0; i < MAP_HEIGHT; i++) {
    for (int j = 0; j < MAP_WIDTH; j++) {
      droneMap[j][i] = 1;
    }
  }

  // Read coordinates from drones
  writeInfoLog(fdlogInfo, "[MASTER] Reading from sockets (drones)");
  while (isRunning) {
    for (int i = 0; i < NUM_DRONES; i++) {
      droneCoords[i][0] = socketRead(fdDrone[i], sizeof(int), fdlogErr);
      droneCoords[i][1] = socketRead(fdDrone[i], sizeof(int), fdlogErr);

      displayText("[MASTER] Message received: ", TEXT_DELAY);
      write(fdDrone[i], "[MASTER] Packet received", 255);
    }

    // Updating map (skipping walls)
    for (int y = 1; y < MAP_HEIGHT-1; y++) {
      // For each row
      for (int x = 1; x < MAP_WIDTH-1; x++) {
        // For each column
        isOccupied = false;
        for (int k = 0; k < NUM_DRONES; k++) {
          // For each drone
          // Check cell occupancy
          if (droneCoords[k][0] == x && droneCoords[k][1] == y) {
            // Cell occupied
            droneMap[x][y] = 2;
            isOccupied = true;
          } else if (!isOccupied) {
            // Cell free
            droneMap[x][y] = 0;
          }
        }
      }
    }

    // Draw map to terminal
    drawMap(droneMap);
    break;
  }

  return 0;
}

void drawMap(int droneMap[82][42]) {
  clearTerminal();

  for (int y = 0; y < MAP_HEIGHT; y++) {
    // For each row
    for (int x = 0; x < MAP_WIDTH; x++) {
      // For each column
      // Draw the appropriate symbol
      switch (droneMap[x][y]) {
        case 1: // wall
          printf("#");
          break;
        case 2: // drone flying
          printf("X");
          break;
        case 3: // drone refueling
          printf("R");
          break;
        default: // empty
          printf(" ");
          break;
      }
    }

    printf("\n");
  }

  fflush(stdout);
}
