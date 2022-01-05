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
  int returnMSG;
  char *msg2server = "Hello, world from group 2!\n";
  printf("Sending msg......");
  //write how many bytes that server should recieve -------
  // ################################################
  if (write(sockfd, msg2server, sizeof(msg2server)) < 0 ){
    perror("sending failed...");
    exit(0);
  }
  printf("Sucess\n");
  if (read(sockfd, &returnMSG, sizeof(int)) < 0 ){
    perror("reading failed...");
    exit(0);
  }
  printf("From Server : %d\n", ntohl(returnMSG));
}

int main()
{
  //setup the server connection
  int sockfd, connfd, opt = 1;
  struct sockaddr_in servaddr, cli;

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errorPrompt (sockfd, "socket creation failed...");

  printf("Socket successfully created..\n");
  //add reuse socket addr and port
  errorPrompt (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)), "setsockopt");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(HOSTNAME); //HOSTNAME ->"127.0.0.1"
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
  }
}
