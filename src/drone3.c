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
#include "../include/utils.h"
#include "../include/values.h"
# define BUFFSIZE 8192

//debug tools and function are commented with "debug" written near them
//the other comments are our implementation of 3D movement, which would need a different master

int n=3;
int x, y;// z;
int * old_x;
int * old_y;
//int * old_z;
int percent=0;
int k;
int m;
int num;
const int full = 300; 
int fuel=full;
int directions [] = { 0, 1, 2, 3, 4, 5, 6, 7 };
//array for 3-dimentional movement implementation 
//int directions [] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25};
int direction=632;
bool change=true;


 
int main(int argc, char *argv[]) {

time_t t;
srand((unsigned) time(&t));
const int portno =PORTNO + 3;
int opt = 1;
int sock = 0;
int coverage[80][40];
struct sockaddr_in serv_addr;
struct hostent *server;
const char * hostname = HOSTNAME;
int response=1;

int fdlogInfo;


fdlogInfo = openInfoLog();

//creation of a matrix of zeros only, that will be used to know the coverage of the map
for(int i=0; i<=80; i++){
    for(int j=0; j<=40; j++){
    
        coverage[i][j]=0;
        
    }
 
}


    //creation of the socket and connection
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
       
   
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");fflush(stdout);
        sleep(30);
        return -1;
    }
   
    x=START3[0]; y=START3[1];//z=START4[2]; //starting position of the drone in the map
    printf("starting x,y: (%d , %d) \n",x,y); fflush(stdout);
    old_x = (int*)calloc(full, sizeof(int));
    old_y = (int*)calloc(full, sizeof(int));
    //old_z = (int*)calloc(full, sizeof(int));
    int steps=0;
    int cnt=0;
    writeInfoLog(fdlogInfo, "[DRONE3] Starts to move");
    while(1){
    	
    	
    	old_x[steps]=x; old_y[steps]=y;//old_z[steps]=z;    //array for keep in to account the trajectory of the drone
	if(fuel>0){ 
	
	if(change){
	        
		 num = ((int)rand()) % 8;                       //random number to choose the direction of the drone
		
		while(num==direction){
		         
			 num = ((int)rand()) % 8;                     //random number to choose a different direction from the old one
		}
		
		printf("current direction: %d \n",directions[num]); fflush(stdout);
		direction=directions[num];
		
	}
        //coordinates calculation
	if((direction==1) || (direction==2) || (direction==3)){
		
		x++;
	}
	else if((direction==5) || (direction==6) || (direction==7)){
		
		x--;
	}
	else{
		x=old_x[steps];
		
	}
	if((direction==7) || (direction==0) || (direction==1)){
		
		y++;
	}
	else if((direction==3) || (direction==4) || (direction==5)){
		
		y--;
	}
	else{
		y=old_y[steps];;
	}
	
	
	//coordinates calculation in case of 3D movement
	/*if((direction==1) || (direction==2) || (direction==3) || (direction==8) || (direction==9) || (direction==10) || 
	   (direction==17)|| (direction==18) || (direction==19)){
		//old_x=x;
		x++;
	}
	else if((direction==5) || (direction==6) || (direction==7) || (direction==3) || (direction==3) || (direction==3) ||
	        (direction==3) || (direction==3) || (direction==3)){
		//old_x=x;
		x--;
	}
	else{
		x=old_x;
	}
	if((direction==7) || (direction==0) || (direction==1) || (direction==14) || (direction==15) || (direction==8) || 
	   (direction==23) || (direction==24) || (direction==17)){
		//old_y=y;
		y++;
	}
	else if((direction==3) || (direction==4) || (direction==5) || (direction==10) || (direction==11) || (direction==12) ||
	        (direction==19) || (direction==20) || (direction==21)){
		//old_y=y;
		y--;
	}
	else{
		y=old_y;
	}
	
	if((direction==8) || (direction==9) || (direction==10) || (direction==11) || (direction==12) || (direction==13) || 
	   (direction==14) || (direction==15) || (direction==16)){
		//old_y=y;
		z++;
	}
	else if((direction==17) || (direction==18) || (direction==19) || (direction==20) || (direction==21) || (direction==22) ||
	        (direction==2) || (direction==24) || (direction==25)){
		//old_y=y;
		z--;
	}
	else{
		z=old_z;
	}*/
	
	//send the coordinates and receive the master's response
	writeInfoLog(fdlogInfo, "[DRONE3] Send coordinates");
	printf("may I go to (%d , %d)? answer 1 (yes) or 0 (no) \n",x,y); fflush(stdout);
	write(sock , &x, sizeof(int));
    	//printf("Hello message 1 sent\n");fflush(stdout);   //debug
    	write(sock , &y, sizeof(int));
    	//printf("Hello message 2 sent\n");fflush(stdout);   //debug
    	/*write(sock , &z, sizeof(int));
    	printf("Hello message 3 sent\n");fflush(stdout);*/
    	writeInfoLog(fdlogInfo, "[DRONE3] Receive master response ");
    	recv( sock , &response, sizeof(response), 0);
    	printf("the answer was: %d\n",response);fflush(stdout);
    	
    	
    	
    	//decreases fuel when the drone moves
	if(response==1){
	//writeInfoLog(fdlogInfo, "[DRONE3] Moving");    //debug
	fuel--;
	printf("fuel remaining: %d/%d \n", fuel,full);fflush(stdout);
	
	//map coverage
	//printf("%d \n", coverage[x][y]);fflush(stdout);    //debug
	if(coverage[x][y]==0){         
	coverage[x][y]=1;
        percent++;
        
	}
	
	steps++;
	printf("The drone covered %d/3200 of the map \n",percent);fflush(stdout);
	
	//if three movements in the same direction are carried out change direction
	if(cnt<(n-1)){ change=false; cnt++; }
	else{ change=true; cnt=0; }
	sleep(0.2);                    //five movements per second
	} 
	
	else{ 
	x=old_x[steps]; y=old_y[steps]; //z=old_z[steps]; 
	change=true; cnt=0;
	}
			
	}
	
	//landing and refueling when fuel runs out
	else{
	printf("Fuel tank is empty. Landing at (%d , %d). \n",x,y); fflush(stdout);//refueling == send to the master the current position
	writeInfoLog(fdlogInfo, "[DRONE3] Landing and refueling");
	send(sock , &x, sizeof(int) , 0 );
    	//printf("hello message 1 sent\n");fflush(stdout);     //debug
    	send(sock , &y, sizeof(int) , 0 );
    	//printf("hello message 2 sent\n");fflush(stdout);     //debug
    	/*write(sock , &z, sizeof(int));
    	printf("Hello message 3 sent\n");fflush(stdout);*/
    	recv( sock , &response, sizeof(response), 0);
    	printf("the answer was: %d\n",response);fflush(stdout);
    	//at each refueling the path the drone went through is displayed
    	printf("the trajectory was: \n");fflush(stdout);
    	for(m=0; m<steps; m++){
    	printf("(%d,%d); ",old_x[m],old_y[m]);fflush(stdout);
    	}
    	printf("\n");fflush(stdout);
    	steps=0;
    	printf("refueling...\n");fflush(stdout);
    	//sleep(10);                        //in case you want a more realistic refueling process
    	fuel=full;
    	printf("fuel is at the maximum capacity: %d/%d \n", fuel,full);fflush(stdout);
    	
	}
    	printf("\n");fflush(stdout);
    
   
    
    
    }
    
    return 0;
    }
