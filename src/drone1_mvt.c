
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  
#include "drone1_com.c"

int main(int argc, char *argv[]){

    // The 8 possible directions 
    const int INCREMENTS[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1, -1}, {1, 0}, {1,-1}};
    // Initialisation of the positions
    struct CoordinatePair current_pos = START1;
    struct CoordinatePair next_pos;
    //random fuel value between 150 and 250
    int fuel = (int)rand()%100 +150;

    srand(time(NULL)); 
    struct timespec remaining, request = {2, TIMESTEP}; //!!!! TO MODIFY!!! seconds = 0 normally

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
        
            next_pos.x = current_pos.x + INCREMENTS[inc][0];
            next_pos.y = current_pos.y + INCREMENTS[inc][1];   
            

            //Send the requested next position for approval
            
            if (fuel>0)
                response = send_message(sock,next_pos.x,next_pos.y);
            else
                response = send_message(sock,current_pos.x,current_pos.y);
                 

            //Reception of the answer
            printf("response ====> %i\n", response);
            if (response == MASTER_OK){
                if (fuel>0){ 
                    current_pos = next_pos;
                    fuel--;
                }
                nb_colisions = 0;
                printf("communication OK\n");
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