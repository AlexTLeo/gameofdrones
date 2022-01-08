#include "../includes/values.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#define GROUPNUM 2
#define SA struct sockaddr

void errorPrompt(int state,char error_msg[])
{
  if (state < 0){
    perror(error_msg);
    exit(EXIT_FAILURE);
  }
}

void sendAndReceive(int sockfd)
{
  char returnMSG[256];
  char *msg2server = "Hello, world from group 2";
  //msg2server = "Hello world from group 2";
  printf("Sending msg......");
  //write how many bytes that server should recieve -------
  // ################################################
  if (write(sockfd, msg2server, 256) < 0 ){
    perror("sending failed...");
    exit(0);
  }
  printf("Sucess\n");
  if (read(sockfd, returnMSG, 255) < 0 ){
    perror("reading failed...");
    exit(0);
  }
  printf("From Server : %s\n", returnMSG);
}

int main()
{
  //setup the server connection
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;
  struct hostent *server;
  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errorPrompt (sockfd, "socket creation failed...");

  printf("Socket successfully created..\n");
  //add reuse socket addr and port
  errorPrompt (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)), "setsockopt");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  server = gethostbyname("localhost");
  if(server == NULL ){
    perror("ERROR, no such host");
    close(sockfd);
    exit(0);
  }
  memcpy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
  servaddr.sin_port = htons(PORTNO+2);

  //init the timestep for loop using sleep
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = TIMESTEP;

  while (1){
    errorPrompt(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)), "connection with the server failed...");
    printf("connected to the server..\n");
    // function for chat
    sendAndReceive(sockfd);
    // close the socket
    errorPrompt(close(sockfd), "close failed...");
    errorPrompt(select(1, NULL, NULL, NULL, &tv), "select failed...");
    break;
    //Using select insead of sleep for preciser timing ref: https://stackoverflow.com/questions/3125645/why-use-select-instead-of-sleep
  }
}
