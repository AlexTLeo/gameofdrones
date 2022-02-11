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
 * This is the master to NUM_DRONES other processes called "drones". It receives their
 * packets via TCP sockets and processes them. The packest are coordinates and
 * status messages. The master will display the drones on a map and inform them
 * in case of potential collisions, amongst other things (see README).
 */

 #define NUM_DRONES 5
 #define MAP_WIDTH 82
 #define MAP_HEIGHT 42

/**
 * Draws a map to terminal with drones and map borders visible
 * @param map - array of map tiles 40x80, where each value has a meaning:
 * -1 is "empty"
 * -2 is "occupied, wall",
 * positive integers is "occupied by drone number n, flying",
 * -3 is "occupied, refueling"
 */
void drawMap(int mapFull[MAP_WIDTH][MAP_HEIGHT]);


int main (int argc, char *argv[]) {
  int mapFull[MAP_WIDTH][MAP_HEIGHT]; // Map of everything (walls, drones, etc.)
  int droneCoordsNext[NUM_DRONES][2]; // Each drone's next requested coordinates
  int droneCoordsCurr[NUM_DRONES][2] = {0}; // Each drone's current coordinates
  bool droneIsRefueling[NUM_DRONES] = {false}; // Each drone's refueling status
  bool isRunning = true;
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
  char* temp = malloc(sizeof(char)*256);

  for (int i = 0; i < NUM_DRONES; i++) {
    portno[i] = PORTNO + i;
  }

  fdlogErr = openErrorLog();
  fdlogInfo = openInfoLog();

  writeInfoLog(fdlogInfo, "[MASTER] Creating sockets");

  // Create sockets
  for (int i = 0; i < NUM_DRONES; i++) {
    fdSocket[i] = socketCreate(AF_INET, SOCK_STREAM, 0, fdlogErr);
    socketSetOpt(fdSocket[i], SOL_SOCKET, SO_REUSEPORT, (void*) &optVal, optLen, 2);
  }

  // Configuring NUM_DRONES sockets, loop through each socket
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

    sprintf(temp, "[MASTER] Socket bound to DRONE %c", i + '0');
    writeInfoLog(fdlogInfo, temp);
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
      mapFull[j][i] = -2;
    }
  }

  // Read coordinates from drones
  writeInfoLog(fdlogInfo, "[MASTER] Master program started");
  while (isRunning) {
    for (int i = 0; i < NUM_DRONES; i++) {
      droneCoordsNext[i][0] = socketRead(fdDrone[i], sizeof(int), fdlogErr);
      droneCoordsNext[i][1] = socketRead(fdDrone[i], sizeof(int), fdlogErr);
    }

    // Calculating collisions
    writeInfoLog(fdlogInfo, "[MASTER] Checking drone collision");

    // Calculating collisions
    for (int i = 0; i < NUM_DRONES; i++) {
      bool isCollision = false;
      int nextX1 = droneCoordsNext[i][0];
      int nextY1 = droneCoordsNext[i][1];
      int currX1 = droneCoordsCurr[i][0];
      int currY1 = droneCoordsCurr[i][1];

      // Check if any drones are refueling (i.e. requested movement is current cell)
      if (nextX1 == currX1 && nextY1 == currY1) {
        // CASE: Refueling
        writeInfoLog(fdlogInfo, "[MASTER] A drone is refueling");
        droneIsRefueling[i] = true;
      } else {
        // CASE: Not refueling
        droneIsRefueling[i] = false;
      }

      // Check if drone is trying to move out of the map
      if (nextX1 <= 0 || nextY1 <= 0 || nextX1 >= MAP_WIDTH - 1 || nextY1 >= MAP_HEIGHT - 1) {
        // CASE: Moving out of map
        isCollision = true;
        sprintf(temp, "[MASTER] Drone %c trying to move out of map: sending MASTER_COL", i + '0');
        writeInfoLog(fdlogInfo, temp);
        socketWrite(fdDrone[i], MASTER_COL, sizeof(MASTER_COL), fdlogErr);
        // Set "next coordinates" to be current cell
        nextX1 = currX1;
        nextY1 = currY1;
        droneCoordsNext[i][0] = currX1;
        droneCoordsNext[i][1] = currY1;
      }

      // Check if any drones are requesting the same cell
      for (int j = 0; j < NUM_DRONES; j++) {
        if (i != j) { // (ignore itself)
          int nextX2 = droneCoordsNext[j][0];
          int nextY2 = droneCoordsNext[j][1];

          if (nextX1 == nextX2 && nextY1 == nextY2) {
            // CASE: Collision
            isCollision = true;
            writeInfoLog(fdlogInfo, "[MASTER] Potential collision detected");
            // Stop i-th drone (arbitrary)
            writeInfoLog(fdlogInfo, "[MASTER] Sending MASTER_COL to drone");
            socketWrite(fdDrone[i], MASTER_COL, sizeof(MASTER_COL), fdlogErr);

            // Set "next coordinates" to be current cell
            droneCoordsNext[i][0] = droneCoordsCurr[i][0];
            droneCoordsNext[i][1] = droneCoordsCurr[i][1];
          }
        }
      }

      if (!isCollision) {
        socketWrite(fdDrone[i], MASTER_OK, sizeof(MASTER_OK), fdlogErr);
      }
    }

    // Updating map (skipping walls)
    for (int y = 1; y < MAP_HEIGHT-1; y++) {
      // For each row
      for (int x = 1; x < MAP_WIDTH-1; x++) {
        // For each column
        bool isOccupied = false;
        for (int k = 0; k < NUM_DRONES; k++) {
          // For each drone, recieve each drone's next cell
          if (droneCoordsNext[k][0] == x && droneCoordsNext[k][1] == y) {
            // CASE: Drone on cell
            if (droneIsRefueling[k]) {
              mapFull[x][y] = -3;
            } else {
              mapFull[x][y] = k;
            }

            // Update drone coordinates
            droneCoordsCurr[k][0] = x;
            droneCoordsCurr[k][1] = y;
            isOccupied = true;
          } else if (!isOccupied) {
            // CASE: No drone on cell
            mapFull[x][y] = -1;
          }
        }
      }
    }

    // Draw map to terminal
    drawMap(mapFull);
    // Minimal wait time just in case the program gets too fast
    usleep(100000); // 100 ms
  }

  return 0;
}

void drawMap(int mapFull[MAP_WIDTH][MAP_HEIGHT]) {
  clearTerminal();

  for (int y = 0; y < MAP_HEIGHT; y++) {
    // For each row
    for (int x = 0; x < MAP_WIDTH; x++) {
      // For each column
      // Draw the appropriate symbol
      switch (mapFull[x][y]) {
        case -1: // empty
          printf(" ");
          break;
        case -2: // occupied, wall
          terminalColor(47, 1);
          printf(" ");
          terminalColor(0, 0);
          break;
        case -3: // occupied, refueling
          terminalColor(43, 1);
          terminalColor(30, 1);
          printf("R");
          terminalColor(0, 0);
          break;
        default: // occupied, flying
        // NB: only works if no more than 10 drones
          terminalColor(42, 1);
          terminalColor(30, 1);
          printf("%d", mapFull[x][y]);
          terminalColor(0, 0);
          break;
      }
    }

    printf("\n");
  }

  fflush(stdout);
}
