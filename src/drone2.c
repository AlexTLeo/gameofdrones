#include "../include/values.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#define GROUPNUM 2
#define SA struct sockaddr
#define MINSTEP 3
#define MAXSTEP 3 //The real maxstep is maxstep + minstep
void errorPrompt(int state,char error_msg[])
{
  if (state < 0){
    perror(error_msg);
    exit(EXIT_FAILURE);
  }
}

void randomDir(int maxstep, int arr[])
{
    srand(time(NULL));            // Initialization, should only be called once.
    int newdir = rand() % 8;      // Returns a pseudo-random integer between 0 and RAND_MAX.
    int n = rand() % maxstep + MINSTEP; // Returns a pseudo-random integer between 0 and RAND_MAX.
    arr[0] = newdir;
    arr[1] = n;
}

void position(int x, int y, int newdir, int coordinate[2])
{
    switch (newdir)
    {
    case 0:
        /* up */
        x = x + 2;
        break;
    case 1:
        // upperright
        x = x + 2;
        y = y + 2;
        break;
    case 2:
        // right
        y = y + 2;
        break;
    case 3:
        // bottom right
        x = x - 2;
        y = y + 2;
        break;
    case 4:
        // bottom
        x = x - 2;
        break;
    case 5:
        // bottom left
        x = x - 2;
        y = y - 2;
        break;
    case 6:
        // left
        y = y - 2;
        break;
    case 7:
        // upper left
        x = x + 2;
        y = y - 2;
        break;
    default:
        printf("random: something unexpected happened.\n");
        break;
    }

    coordinate[0] = x;
    coordinate[1] = y;
}

int sendAndReceive(int sockfd, int goTo [])
{
  int returnMSG;
  //msg2server = "Hello world from group 2";
  printf("Sending msg...... goTo %d, %d", goTo[0], goTo[1]);
  //write how many bytes that server should recieve -------
  // Sending co-ordiante to master
  if (write(sockfd, goTo, sizeof(int)*2) < 0 ){
    perror("sending failed...");
    exit(0);
  }
  printf("Send successfully\n");
  // Recieve the permission
  if (read(sockfd, &returnMSG, sizeof(int)*1) < 0 ){
    perror("reading failed...");
    exit(0);
  }
  printf("From Server : %d\n", returnMSG);

  if(returnMSG == MASTER_COL){
    printf("Rejected by Master (rejected code: %d), unable to move to %d, %d", returnMSG, goTo[0], goTo[0]);
    return 0;
  }
  else if (returnMSG == MASTER_OK){
      printf("Approved by Master (code: %d), moving to %d, %d", returnMSG, goTo[0], goTo[0]);
      return 1;
  }
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


  int dir = 0;
  int step = 0;
  int res =0;
  int coordinate[2], trajectory[2], goTo[2];
  int fuel = 500;
  //coordinate = values.h
  coordinate[0] = 30;
  coordinate[1] = 30;
  trajectory[1] = 0;
  errorPrompt(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)), "connection with the server failed...");
  printf("connected to the server..\n");
  while(1){
    if (fuel == 0){
      // Sending to master
      sleep(4);
      fuel = 500;
    }
    if (trajectory[1]==0){
      randomDir(MAXSTEP, trajectory);
    }
    printf("coordinates %d, %d", coordinate[0], coordinate[1]);
    position(coordinate[0], coordinate[1], trajectory[0], goTo);
    printf("-------------- %d, %d", goTo[0], goTo[1]);

    for (int i=0; i<= DRONE_TIMEOUT; i++){ //this one i <= Timeout
      // errorPrompt(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)), "connection with the server failed...");
      // printf("connected to the server..\n");
      res = sendAndReceive(sockfd, goTo);
      // errorPrompt(close(sockfd), "close failed...");
      // printf("Close successfully..\n");
      if(res==1){
        position(coordinate[0], coordinate[1], trajectory[0], coordinate);
        fuel --;
        trajectory[1] --;
        break;
      }
      else{
        printf("Permission denied (Attempt: %d)\n", i);
        errorPrompt(select(1, NULL, NULL, NULL, &tv), "select failed...");
      }
    }

    if(res==0){
      printf("Timeout\n");
      randomDir(MAXSTEP, trajectory);
    }
    errorPrompt(select(1, NULL, NULL, NULL, &tv), "select failed...");
    //Using select insead of sleep for preciser timing ref: https://stackoverflow.com/questions/3125645/why-use-select-instead-of-sleep
  }
  errorPrompt(close(sockfd), "close failed...");
  printf("Close successfully..\n");
  return 0;
}
