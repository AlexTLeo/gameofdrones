#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>

#include "../include/utils.h"

void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[]) {
  int sockfd, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  if (argc < 2) {
    fprintf(stderr,"usage %s hostname port\n", argv[0]);
    exit(0);
  }

  portno = atoi(argv[1]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    error("ERROR opening socket");
  }

  server = gethostbyname("localhost");

  if (server == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }

  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    error("ERROR connecting");
  }

  char buffer[256];
  char * bufferW = "Test";

  n = write(sockfd,bufferW,strlen(bufferW));

  if (n < 0) {
    error("ERROR writing to socket");
  }

  bzero(buffer,256);
  n = read(sockfd,buffer,255);

  if (n < 0) {
    error("ERROR reading from socket");
  }

  printf("%s\n",buffer);
  fflush(stdout);

  return 0;
}
