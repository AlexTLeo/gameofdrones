
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
//#include "drone1_com.h"
#include "../include/values.h"

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
    int message[2] = {x,y};
    //char message[] = "blabla";

    int server_reply;
     //Send message
        if (write(sock, message, sizeof(message)) < 0)
        {
            puts("Client: Send failed");
            return 2;
        }

        //Receive a reply from the server
        if (read(sock, &server_reply, 2 * sizeof(message)) < 0)
        {
            puts("Client: recv failed");
            return 2;
        }
    printf("response server%i\n", server_reply);
    int response = server_reply;
    return response;
}

int main(int argc, char *argv[]){

    // The 8 possible directions 
    const int INCREMENTS[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1, -1}, {1, 0}, {1,-1}};
    // Initialisation of the positions
    int current_pos[2];
    current_pos[0] = 20;//START1[0];
    current_pos[1] = 20; //START1[1];
    int next_pos[2];
    //random fuel value between 150 and 250
    int fuel = (int)rand()%100 +150;

    srand(time(NULL)); 
    struct timespec remaining, request = {5, TIMESTEP}; // TO MODIFY, 5-->0

    int sock;
    int response;
    if (create_and_connect_to_server(&sock) == 1){
        printf("connect error\n");
    }
    

    printf("Started loop..\n");
    while (1) {
        
        // random error between 0 and 7
        int inc = (int)rand()%8;
        //random number of steps between 3 and 10
        int max_steps = (int)rand()%8 +3; 
        int current_step = 0;
        int nb_colisions = 0;
        

        printf("Trajectory set : %d steps, in the direction %d\n", max_steps, inc);

        while (current_step<max_steps){
        
            next_pos[0] = current_pos[0] + INCREMENTS[inc][0];
            next_pos[1] = current_pos[1] + INCREMENTS[inc][1];   
            

            //Send the requested next position for approval
            
            if (fuel>0){
                response = send_message(sock,next_pos[0],next_pos[1]);
                printf("mouvement demandé : x = %d, y = %d\n", next_pos[0],next_pos[1]);
            }
            else{
                response = send_message(sock,current_pos[0],current_pos[1]);
                printf("mouvement demandé : x = %d, y = %d\n", next_pos[0],next_pos[1]);
            }
                 

            //Reception of the answer
            printf("response ====> %i\n", response);
            if (response == MASTER_OK){
                if (fuel>0){ 
                    current_pos[0] = next_pos[0];
                    current_pos[1] = next_pos[1];
                    fuel--;
                }
                nb_colisions = 0;
                printf("============================================>communication OK\n");
            }
            else if (response == MASTER_COL){
                nb_colisions++;
                printf("a collision happend, %d in a row\n", nb_colisions);
            }
            else{
                printf("socket error for send\n");
            }

            // Handling of succesive colisions
            if (nb_colisions==DRONE_TIMEOUT)
                current_step = max_steps;

            current_step++;

                       
            nanosleep(&request, &remaining);
        }    
        
    }




}