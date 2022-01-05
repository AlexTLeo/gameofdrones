#include "../includes/values.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

int main(int argc, char *argv[]){

    const int INCREMENTS[8][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1, -1}, {1, 0}, {1,-1}};
    srand(time(NULL)); 
    struct timespec remaining, request = {2, TIMESTEP};

    struct CoordinatePair current_pos = START1;
    struct CoordinatePair next_pos;
    //random fuel value between 150 and 250
    int fuel = (int)rand()%100 +150;
    

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
        
            //printf("current position : %d, %d\n",current_pos.x, current_pos.y);
            next_pos.x = current_pos.x + INCREMENTS[inc][0];
            next_pos.y = current_pos.y + INCREMENTS[inc][1];   
            //printf("next position : %d, %d\n",next_pos.x, next_pos.y);

            int response =2;
            if (fuel>0)
                //response = send(next_pos);
                response =0;
            else
                //response = send(current_pos);
                response = 0;
                 

            
            //attention what happend if landing ?
            if (response == MASTER_OK){
                if (fuel>0){ 
                    current_pos = next_pos;
                    fuel--;
                }
                //printf("communication OK\n");
            }
            else if (response == MASTER_COL){
                nb_colisions++;
                printf("a collision happend, %d in a row\n", nb_colisions);
            }
            else{
                printf("communication between drone 1 and master failed\n");
                exit(EXIT_FAILURE);
            }


            if (nb_colisions==DRONE_TIMEOUT)
                current_step = max_steps;

            current_step++;

            printf("current step is %d \n", current_step);
            
            nanosleep(&request, &remaining);
        }    
        
    }




}