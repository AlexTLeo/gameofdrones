/**
 * ARP - Advanced Robot Programming
 * Assignment 3 - Drone simulation
 * 
 * @file drone4.c
 * @author @simone-contorno - @GabrieleRusso11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/**
 * Headers
 */
#include "../include/values.h"
#include <stdio.h>
#include <netdb.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Global variables 
 */
int sockfd; // Socket file descriptor

/**
 * @brief Return an error message and exit.
 * 
 * @param msg 
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

/**
 * @brief Generate a random integer number in 
 * a specific interval.
 * 
 * @param lower 
 * @param upper 
 * @return int 
 */
int randomNumberInt(int lower, int upper) {
    srand(time(NULL));
    return rand() % (upper - lower + 1) + lower;
}

/**
 * @brief Compute next position.
 * 
 * @param direction 
 */
void nextPosition(int direction) {
    /**
     * 0 = Up
     * 1 = Up and Left
     * 2 = Left
     * 3 = Down and Left
     * 4 = Down
     * 5 = Down and Right
     * 6 = Right
     * 7 = Up and Right
     */

    /**
     * Update x 
     */
    if (direction == 5 || direction == 6 || direction == 7)
        coordinatePair[0] += 1;
    else if (direction == 1 || direction == 2 || direction == 3)
        coordinatePair[0] -= 1;

    /**
     * Update y 
     */
    if (direction == 0 || direction == 1 || direction == 7)
        coordinatePair[1] += 1;
    else if (direction == 3 || direction == 4 || direction == 5)
        coordinatePair[1] -= 1;
    
}

/**
 * @brief Create a socket connection.
 * 
 * @param portno 
 * @param hostname 
 */
void socketConnection(const int portno, const char * hostname) {
    struct sockaddr_in serv_addr; // Server address
    struct hostent *server; // Host computer on the Internet

    // Set server hostname
    server = gethostbyname(hostname);
    if (server == NULL) 
        error("ERROR, no such host");

    // Creates a new socket on the Internet
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd < 0) 
        error("ERROR opening socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        error("ERROR setsockopt(SO_REUSEADDR) failed");
    
    // Set the addresses
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET; // Address family

    bcopy((char *) server->h_addr, // Client address
        (char *) &serv_addr.sin_addr.s_addr, // Server address
        server->h_length); // Length of address

    serv_addr.sin_port = htons(portno); // Convert port number into network byte order

    // Start socket connection to the Server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR connecting to the server");
}

/**
 * @brief main function.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char * argv[]) {
    // Take data from includes/values.h file
	coordinatePair[0] = START4[0];
	coordinatePair[1] = START4[1];

    const int portno = PORTNO + 4; // Port number
    const char * hostname = HOSTNAME; // Hostname

    // Generate a random drone's battery power 
	int power = randomNumberInt(100, 300);

    // Create socket connection with the Master process
    socketConnection(portno, hostname);
    
    /**
     * Move the drones until the battery is not discharge
     */
    int response;

    while (power > 0) {
        // Generate a random direction and a random number of steps 
        int direction = randomNumberInt(0, 7);
        int steps = randomNumberInt(5, 10);

        printf("\nDirection: %d\nSteps: %d\n", direction, steps);
        fflush(stdout);

        // Move the drone 
        for (int i = 0; i < steps; i++) {
            // Compute the next position coordinates
            nextPosition(direction);

            printf("X: %d\nY: %d\n", coordinatePair[0], coordinatePair[1]);
            fflush(stdout);

            // Send the coordinates to the Master 
            if (send(sockfd, &coordinatePair[0], sizeof(int), 0) < 0)
		        error("ERROR sending the coordinate x to the master");
            if (send(sockfd, &coordinatePair[1], sizeof(int), 0) < 0)
		        error("ERROR sending the coordinate y to the master");

            // Read Master response and manage it
            if (recv(sockfd, &response, sizeof(int), 0) < 0) 
                error("ERROR reading the master response");
            printf("Master response: %d\n", response);
            fflush(stdout);

            if (response == MASTER_OK) { // Success
                power--;
                printf("Movement allowed.\nPower remained: %d\n", power);
                fflush(stdout);
            }
            else if (response == MASTER_COL) { // Fail
                i--;
                printf("Movement not allowed.\nPower remained: %d\n", power);
                fflush(stdout);
                sleep(DRONE_TIMEOUT);
            }
            else {
                printf("No response received by the Master process.\nPower remained: %d\n", power);
                fflush(stdout);
            }

            usleep(TIMESTEP * 5000);
        }
    }

    // Close socket connection and return 
    close(sockfd); 
    return 0;
}