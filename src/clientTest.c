#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "../include/utils.h"

void error(char *msg) {
  perror(msg);
  exit(0);
}

// Robert Jenkins' 96 bit Mix Function
unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a=a-b;  a=a-c;  a=a^(c >> 13);
    b=b-c;  b=b-a;  b=b^(a << 8);
    c=c-a;  c=c-b;  c=c^(b >> 13);
    a=a-b;  a=a-c;  a=a^(c >> 12);
    b=b-c;  b=b-a;  b=b^(a << 16);
    c=c-a;  c=c-b;  c=c^(b >> 5);
    a=a-b;  a=a-c;  a=a^(c >> 3);
    b=b-c;  b=b-a;  b=b^(a << 10);
    c=c-a;  c=c-b;  c=c^(b >> 15);
    return c;
}

int main(int argc, char *argv[]) {
  int sockfd, portno, n;

  struct sockaddr_in serv_addr;
  struct hostent *server;

  char buffer[256];
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

  const int optVal = 1;
  const socklen_t optLen = sizeof(optVal);
  socketSetOpt(sockfd, SOL_SOCKET, SO_REUSEPORT, (void*) &optVal, optLen, 2);
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
    (char *)&serv_addr.sin_addr.s_addr,
    server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
    error("ERROR connecting");
  }

  srand(mix(clock(), time(NULL), getpid()));
  int a = rand() % 80 + 1;
  int b = rand() % 40 + 1;

  printf("%d %d \n", a, b);
  socketWrite(sockfd,a,sizeof(a), 2);
  socketWrite(sockfd,b,sizeof(b), 2);

  if (n < 0) {
    error("ERROR writing to socket");
  }

  bzero(buffer,256);
  n = read(sockfd,buffer,255);

  if (n < 0) {
    error("ERROR reading from socket");
  }

  printf("%s\n",buffer);
  return 0;
}
