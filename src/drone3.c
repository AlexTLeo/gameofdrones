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
#include <include/values.h> 

#define BUFFSIZE 8192
#define PORT 4003

 
int main(int argc, char *argv[]) {

int opt = 1;
char str[BUFFSIZE];
char format_string[80]="%d";
int sock = 0, valread;
struct sockaddr_in serv_addr;


  

char *hello="Hello from client";;






    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
     
     if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    while(1){
    
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );fflush(stdout); 
    
    send(sock , hello, strlen(hello) , 0 );  //strlen(hello)
    printf("Hello message sent\n");fflush(stdout);
    
    }
    sleep(60);
    return 0;
    }
