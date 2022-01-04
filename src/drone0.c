#include "../includes/values.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  

int main(int argc, char *argv[]) {
    // Max number of steps in the same direction
    const int same_direction = 5;
    // Get starting position
    struct CoordinatePair pos = START0;
    // Timeout counter
    int timeout = 0;
    // Initialize response variable
    int response = 1;
    // Intializes random number generator
    time_t t;
    srand((unsigned) time(&t));
    for (int i = 0; i < STEPS; i++) {
        // Reset timeout counter
        timeout = 0;
        // Choose new direction
        int direction = rand() % 8;
        for (int j = 0; j < same_direction; j++) {
            // Update position
            if (direction == 0) {
                // Up 
                printf("Up ");
                pos.y++;
            } else if (direction == 1) {
                // Up - right
                printf("Up - right ");
                pos.x++;
                pos.y++;
            } else if (direction == 2) {
                // Right
                printf("Right ");
                pos.x++;
            } else if (direction == 3) {
                // Down -right
                printf("Down - right ");
                pos.x++;
                pos.y--;
            } else if (direction == 4) {
                // Down
                printf("Down ");
                pos.y--;
            } else if (direction == 5) {
                // Down - left
                printf("Down - left ");
                pos.x--;
                pos.y--;
            } else if (direction == 6) {
                printf("Left ");
                // Left
                pos.x--;
            } else if (direction == 7) {
                // Up - left
                printf("Up - left ");
                pos.x--;
                pos.y++;
            }
            printf("(%d, %d)\n", pos.x, pos.y);
            fflush (stdout);
            sleep(1);
            
            // TODO: send new position to master and wait for response (if response is collision, then update timeout and check condition on it)
            if (response == 0) {
                timeout ++;
                if (timeout == DRONE_TIMEOUT) {
                    // If drone has collided "timeout" times, then change direction
                    break;
                }
            } else if (response == 1) {
                // No collision found
                continue;
            } else {
                // Invalid response
                perror("Response not valid");
                exit(EXIT_FAILURE);
            }
        }
    }   
    return 0;
}