#include "../includes/values.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/socket.h> 
#include <netdb.h> 
#include <string.h>

int main(int argc, char *argv[]) {

	// Initialize socket file descriptors, port number and client address
	int sockfd, newsockfd, clilen, n;
	// Struct for addresses
	struct sockaddr_in serv_addr, cli_addr;
	// Create new socket and check for errors (SOCK_STREAM for continous byte stream, as in pipes)
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) { 
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}
	// Set socket to close after process execution
	int true = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(int));
	// Set address struct fields
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORTNO);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// Bind socket to address
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(EXIT_FAILURE);
	}
	// Wait for connection
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	// Accept new connections and check for errors
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) {
		perror("ERROR on accept");
		exit(EXIT_FAILURE);
	}
    // Init dynamic array
    int array[2];
    while(1){
        // Read array from socket
        n = recv(newsockfd, array, sizeof(array), MSG_WAITALL);
        if (n < 0) {
            printf("ERROR reading from socket\n");fflush(stdout);
            exit(EXIT_FAILURE);
        }
        printf("drone1: Received: %d,%d, sending 0\n", array[0], array[1]);
        fflush(stdout);

        // Const response 
        int not_ok = 1;
        n = write(newsockfd, &not_ok, sizeof(not_ok));
        if (n < 0) {
            printf("ERROR reading from socket\n"); fflush(stdout);
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}