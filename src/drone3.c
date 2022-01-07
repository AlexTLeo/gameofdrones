#include <stdio.h>
#include <string.h>
#include <fcntl.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/values.h"



 
int main(int argc, char *argv[]) {
const int portno =PORTNO + 3;
int opt = 1;

int sock = 0;
struct sockaddr_in serv_addr;
struct hostent *server;
const char * hostname = HOSTNAME;
char *hello="[DRONE3]Hello from drone3";
char buffer[256];

server = gethostbyname(hostname);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");fflush(stdout);
        sleep(30);
        return -1;
    }
    
    
    
     
     if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &(int){1}, sizeof(int)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
       
    }
       
    bzero((char *) &serv_addr, sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr,                  // Client address
        (char *) &serv_addr.sin_addr.s_addr,       // Server address
        server->h_length);                        // Length of address
    serv_addr.sin_port = htons(portno);  
       
    
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    while(1){
    send(sock , hello, strlen(hello) , 0 );  //strlen(hello)
    printf("Hello message sent\n");fflush(stdout);
    
    recv( sock , buffer, strlen(buffer)-1, 0);
    printf("%s\n",buffer );fflush(stdout); 
    
    
    
    }
    sleep(60);
    return 0;
    }
