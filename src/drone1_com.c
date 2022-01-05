#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "../includes/values.h"
#define LENGTH_MSG 8

int create_and_connect_to_server(int *sock){
    //Create socket
     struct sockaddr_in server;
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        printf("Client: Could not create socket");
        fflush(stdout);
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTNO+1);

    //Connect to remote server
    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Client: connect failed. Error");
        return 1;
    }
    return 0;
}

int send_message(int sock,int x,int y){
    char message[80];
    snprintf(message, sizeof(message), "%i,%i", x,y);

    char *server_reply;
    if (NULL == (server_reply = malloc(2 * LENGTH_MSG)))
    {
        perror("malloc error");
    }
     //Send message
        if (send(sock, message, strlen(message), 0) < 0)
        {
            puts("Client: Send failed");
            return 2;
        }

        //Receive a reply from the server
        if (recv(sock, server_reply, 2 * strlen(message), 0) < 0)
        {
            puts("Client: recv failed");
            return 2;
        }
    int response = atoi(server_reply);
    free(server_reply);
    return response;
}

struct CoordinatePair get_coords(int sock){
    struct CoordinatePair coords;
     char *server_reply;
    if (NULL == (server_reply = malloc(2 * LENGTH_MSG)))
    {
        perror("malloc error");
    }
      //Receive a reply from the server
        if (recv(sock, server_reply, 2 * LENGTH_MSG, 0) < 0)
        {
            puts("Client: recv failed");
        }
    coords.x = atoi(strsep(&server_reply, ","));
    coords.y = atoi(strsep(&server_reply, ","));
    free(server_reply);
    return coords;
}