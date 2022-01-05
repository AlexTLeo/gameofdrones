#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "../includes/values.h"
#define LENGTH_MSG 8


int main(int argc, char *argv[])
{
	int socket_desc, client_sock, c, read_size;
	struct sockaddr_in server, client;
	char *client_message;
	if (NULL == (client_message = malloc(2 * LENGTH_MSG))){
	}

	//Create socket
	socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc == -1)
	{
		printf("Server: Could not create socket");
	}
	puts("Server: Socket created");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORTNO+1);

	//Bind
	if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		//print the error message
		perror("Server: bind failed. Error");
		exit(EXIT_FAILURE);
	}

	//Listen
	listen(socket_desc, 3);

	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);

	//accept connection from an incoming client
	client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);
	if (client_sock < 0)
	{
		perror("Server: accept failed");
		exit(EXIT_FAILURE);
	}
	puts("Server: client connected");
	while ((read_size = recv(client_sock, client_message, 15, 0)) > 0)
	{
		//Send the message back to client
        int test = 1;
        char test_char = test + '0';
        //char test_char[] = "15,10";
		//write(client_sock, &test_char, 10);
        write(client_sock, &test_char, 10);
	}
	free(client_message);

	puts("Server: terminated");

	if (read_size == 0)
	{
		puts("Server: Client disconnected");
		fflush(stdout);
	}
	close(client_sock);

	return 0;
}