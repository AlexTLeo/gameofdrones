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
#include <sys/socket.h>
#include <netinet/in.h>

/**
 * Global variables 
 */
int sockfd; // Socket file descriptor

/**
 * @brief Return an error message and exit
 * 
 * @param msg 
 */
void error(char *msg) {
    perror(msg);
    exit(0);
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
 * @brief main function
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char *argv[]) {
    const int portno = PORTNO + 4; // Port number
    const char * hostname = HOSTNAME; // Hostname

    // Create socket connection with the Master process
    socketConnection(portno, hostname);

    // Send data to the Master  
	char * bufferW = "[DRONE 4] Hello!";
    if (send(sockfd, bufferW, strlen(bufferW), 0) < 0)
		error("ERROR socket writing");

    // Read message from the Master
	char bufferR[256];
    bzero(bufferR, strlen(bufferR));
    if (recv(sockfd, bufferR , strlen(bufferR)-1, 0) < 0) 
        error("ERROR socket reading");
	
    printf("[DRONE 4] Message received: %s\n", bufferR);
    fflush(stdout);

    close(sockfd); 

    return 0;
}