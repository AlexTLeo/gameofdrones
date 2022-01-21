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
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/values.h"
# define BUFFSIZE 8192

//----------------------------------
int n=3;
int x, y;
int old_x, old_y;
int fuel=10; //300;
int directions [] = { 0, 1, 2, 3, 4, 5, 6, 7 };
int direction=632;
bool change=true;
char str[BUFFSIZE];
char str2[BUFFSIZE];
char str3[BUFFSIZE];
//----------------------------------

 
int main(int argc, char *argv[]) {
srand(time(NULL));
const int portno =PORTNO + 3;
int opt = 1;

int sock = 0;
struct sockaddr_in serv_addr;
struct hostent *server;
const char * hostname = HOSTNAME;
char *hello="[DRONE3]Hello from drone3";
char buffer[256];
//int buffer[2];

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
    bcopy((char *) server->h_addr, // Client address
        (char *) &serv_addr.sin_addr.s_addr, // Server address
        server->h_length); // Length of address
    serv_addr.sin_port = htons(portno);  
       
    // Convert IPv4 and IPv6 addresses from text to binary form
    /*if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");fflush(stdout);
        sleep(30);
        return -1;
    }*/
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    x=40; y=80;
    printf("starting x,y: (%d , %d) \n",x,y); fflush(stdout);
    
    int cnt=0;
    while(1){
    	//----------------------------------
    	old_x=x; old_y=y;
	if(fuel>0){ //if(permission){
	if(change){
		int num = ((int)rand()) % 8;//7;
		//while((int num = ((int)rand()) % 8)==old_num){}
		while(num==direction){
			int num = ((int)rand()) % 8;
		}
		printf("current direction: %d \n",directions[num]); fflush(stdout);
		direction=directions[num];
	}
	
	if((direction==1) || (direction==2) || (direction==3)){
		//old_x=x;
		x++;
	}
	else if((direction==5) || (direction==6) || (direction==7)){
		//old_x=x;
		x--;
	}
	else{
		x=old_x;
	}
	if((direction==7) || (direction==0) || (direction==1)){
		//old_y=y;
		y++;
	}
	else if((direction==3) || (direction==4) || (direction==5)){
		//old_y=y;
		y--;
	}
	else{
		y=old_y;
	}
	
	printf("may I go to (%d , %d)? answer 1 (yes) or 0 (no) \n",x,y); fflush(stdout);
	send(sock , x, sizeof(int) , 0 );
    	printf("Hello message 1 sent\n");fflush(stdout);
    	send(sock , y, sizeof(int) , 0 );
    	printf("Hello message 2 sent\n");fflush(stdout);
    	recv( sock , buffer, strlen(buffer)-1, 0);
    	printf("the answer was: %s\n",buffer);fflush(stdout);
    	//read(STDIN_FILENO,str3,BUFFSIZE);
	//printf("string: %s",str3);
	
	//if(str3[0]=='y'){ 
	if(buffer[0]==1){ 
	fuel--; 
	if(cnt<(n-1)){ change=false; cnt++; }
	else{ change=true; cnt=0; }
	sleep(0.2); //1/5
	} 
	
	else{ 
	x=old_x; y=old_y; //it doesn't return to the original values: whyyyy
	change=true; cnt=0;
	}
			
	}
	else{
	printf("Fuel tank is empty. Landing at (%d , %d). \n",x,y); fflush(stdout);
	//send(sock , hello, strlen(hello) , 0 );
	send(sock , x, sizeof(int) , 0 );
    	printf("Hello message 1 sent\n");fflush(stdout);
    	send(sock , y, sizeof(int) , 0 );
    	printf("Hello message 2 sent\n");fflush(stdout);
    	recv( sock , buffer, strlen(buffer)-1, 0);
    	printf("the answer was: %s\n",buffer);fflush(stdout);
	}
    	//----------------------------------
    
    /*
    send(sock , hello, strlen(hello) , 0 );  //strlen(hello)
    printf("Hello message sent\n");fflush(stdout);
    
    recv( sock , buffer, strlen(buffer)-1, 0);
    printf("%s\n",buffer );fflush(stdout); 
    */
    
    
    }
    //sleep(60);
    return 0;
    }
