
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> 

#include "../include/values.h"
#include "../include/utils.h"

#define LENGTH_MSG 8

int create_and_connect_to_server(int *sock, int num_drone){
    //Create socket
     struct sockaddr_in server;
    *sock = socket(AF_INET, SOCK_STREAM, 0);

    //Open erros.log
    int fd_logError = openErrorLog();

    if (*sock == -1)
    {
        writeErrorLog(fd_logError, "[drone1] Client: Could not create socket",1);
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORTNO+num_drone);

    //Connect to remote server
    if (connect(*sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        writeErrorLog(fd_logError, "[drone1] Client: connect failed.",1);
        return 1;
    }
    return 0;
}

int send_message(int sock,int x,int y){
    int message[2] = {x,y};
    //char message[] = "blabla";

    //Open erros.log
    int fd_logError = openErrorLog();

    int server_reply;
     //Send message
        if (write(sock, message, sizeof(message)) < 0)
        {
            //puts("Client: Send failed");
            writeErrorLog(fd_logError, "[drone1] Client: Send failed",2);
            return 2;
        }

        //Receive a reply from the server
        if (read(sock, &server_reply, 2 * sizeof(message)) < 0)
        {
            puts("Client: recv failed");
            writeErrorLog(fd_logError, "[drone 1] Client: recv failed",2);
            return 2;
        }
    printf("response server%i\n", server_reply);
    int response = server_reply;
    return response;
}

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

int main(int argc, char *argv[]){

    // The 8 possible directions 
    const int INCREMENTS[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1, -1}, {1, 0}, {1,-1}};
    // Initialisation of the positions
    int current_pos[2];
    int num_drone = atoi(argv[1]);
    current_pos[0] = START1[num_drone];
    current_pos[1] = START1[num_drone];
    int next_pos[2];
    //random fuel value between 150 and 200
    srand(mix(clock(), time(NULL), getpid()));
    int fuel = (int)rand()%50 +150;
    int tps_refuel = 15;

    //updates every 200ms (TIMESTEP)
    struct timespec remaining, request = {1, TIMESTEP}; 

    //Socket connection
    int sock;
    int response;
    if (create_and_connect_to_server(&sock, num_drone) == 1){
        printf("connect error\n");
    }
    
    //Openning of the log file info
    int fd_logInfo = openInfoLog();
    int fd_logError = openErrorLog();

    printf("Started loop..\n");
    while (1) {
        
        // random error between 0 and 7
        int inc = (int)rand()%8;
        //random number of steps between 3 and 10
        int max_steps = (int)rand()%8 +3; 
        int current_step = 0;
        int nb_colisions = 0;
                
        char msg[80];
        sprintf(msg, "[drone1] Trajectory set : %d steps, in the direction %d\n", max_steps, inc);
        writeInfoLog(fd_logInfo, msg);

        while (current_step<max_steps){
        
            next_pos[0] = current_pos[0] + INCREMENTS[inc][0];
            next_pos[1] = current_pos[1] + INCREMENTS[inc][1];   
            

            //Send the requested next position for approval
            
            if (fuel>0){
                response = send_message(sock,next_pos[0],next_pos[1]);
                printf("mouvement demandé : x = %d, y = %d\n", next_pos[0],next_pos[1]);
            }
            else{ //i.e. fuel=0
                response = send_message(sock,current_pos[0],current_pos[1]);
                tps_refuel--;
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
                else if (tps_refuel <= 0){
                    // refuelling time is over, the tank is full
                    tps_refuel = 15;
                    fuel = (int)rand()%50 +150;
                }
                nb_colisions = 0;
                printf("============================================>communication OK\n");
            }
            else if (response == MASTER_COL){
                nb_colisions++;
                printf("a collision happend, %d in a row\n", nb_colisions);

                //even if the move is not accepted, the fuel decreases
                if (fuel>0)
                    fuel--;                
                else if (tps_refuel <= 0)
                    fuel = (int)rand()%50 +150;
                
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