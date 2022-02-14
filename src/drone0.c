#include "../include/values.h"
#include "../include/utils.h"
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h>
#include <netdb.h> 
#include <libgen.h>
#include <sys/wait.h>
#include <string.h>
#include <math.h>

#define MAP_WIDTH 82
#define MAP_HEIGHT 42

const int MAX_DISTANCE = 100000;
char* string1;
char string2[30];
char* string3;
char string4[30];
char* string5;
int fdlogInfo;
int fdlogError;

unsigned long mix(unsigned long a, unsigned long b, unsigned long c) {
    a=a-b; a=a-c; a=a^(c >> 13);
    b=b-c; b=b-a; b=b^(a << 8);
    c=c-a; c=c-b; c=c^(b >> 13);
    a=a-b; a=a-c; a=a^(c >> 12);
    b=b-c; b=b-a; b=b^(a << 16);
    c=c-a; c=c-b; c=c^(b >> 5);
    a=a-b; a=a-c; a=a^(c >> 3);
    b=b-c; b=b-a; b=b^(a << 10);
    c=c-a; c=c-b; c=c^(b >> 15);
    return c;
}

void bubblesort(double *number, int n){
    double a = 0;
    for (int i = 0; i < n; ++i){
        for (int j = i + 1; j < n; ++j){
            if (number[i] > number[j]){
                a = number[i];
                number[i] = number[j];
                number[j] = a;
            }
        }
    }
}

void log_desired_position(int x, int y){
    string1 = "[Drone0] Desired position (";
    sprintf(string2, "%d", x);
    string3 = ",";
    sprintf(string4, "%d", y);
    string5 = ") ";
    char* log_string = malloc(strlen(string1) + strlen(string2) + strlen(string3) + strlen(string4) + strlen(string5) + 1);
    // Create string to be written to lof file
    strcpy(log_string, string1);
    strcat(log_string, string2);
    strcat(log_string, string3);
    strcat(log_string, string4);
    strcat(log_string, string5);
    writeInfoLog(fdlogInfo, log_string);
}

int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int findmin(int a[],int n) {
    int min, max, i, max_index, min_index;
    min = max = a[0];
    max_index = 0;
    min_index = 0;
    for(i = 1; i < n; i++){
        if(min > a[i]){
            min = a[i]; 
            min_index = i;
        }  
        if(max < a[i]){
            max = a[i];
            max_index = i;
        }       
    }
    return min_index;
}

int getRandomX() {
    int x = rand() % (MAP_WIDTH - 3);
    x++;
    return x;
}

int getRandomY() {
    int y = rand() % (MAP_HEIGHT - 3);
    y++;
    return y;
}

int main(int argc, char *argv[]) {
    // Initialize random number generator
    srand(mix(clock(), time(NULL), getpid()));
    // Get starting position
    int position[2] = {START0[0], START0[1]};
    fdlogInfo = openInfoLog();
    writeInfoLog(fdlogInfo, "[Drone0] Starting...");
    // Socket connection
    // Initialize socket file descriptor and variable to check writing to socket
    int sockfd, n;
    // Intialize struct for server address
    struct sockaddr_in serv_addr;
    struct hostent *server;
    // Create new socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        writeErrorLog(fdlogError, "[Drone0] Error in opening socket", errno);
        exit(EXIT_FAILURE);
    }
    // Set sockopt to reuse the address
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");
    // Server address is local host
    server = gethostbyname(HOSTNAME);   
    // Check server address
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        writeErrorLog(fdlogError, "[Drone0] No such server", errno);
        exit(EXIT_FAILURE);
    }
    // Set to 0 struct for server address
    bzero((char *) &serv_addr, sizeof(serv_addr));
    // Set struct fields
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(PORTNO);
    // Connect to server and check for succesful connection
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        writeErrorLog(fdlogError, "[Drone0] Error in connectig to socket", errno);
        exit(EXIT_FAILURE);
    }
    // Socket connection done
    writeInfoLog(fdlogInfo, "[Drone0] Successfully connected");

    // Timeout counter
    int timeout = 0;
    // Initialize response variable
    int response = 1;
    // Store old position in case of collisions
    int old_position[2];

    // Position goal to reach
    int goal_x = getRandomX();
    int goal_y = getRandomY();
    int x_values[8];
    int y_values[8];
    double distance[8];
    double distance_ordered[8];
    while (1) {
        for (int i = 0; i < STEPS; i++) {
            printf("[Drone0] Goal position: (%d, %d)\n", goal_x, goal_y);
            fflush(stdout);
            // Store old posiiton
            old_position[0] = position[0];
            old_position[1] = position[1];

            // Calculate all possible cells to reach
            // Up
            x_values[0] = position[0];
            y_values[0] = position[1] + 1;
            // Up- right
            x_values[1] = position[0] + 1;
            y_values[1] = position[1] + 1;
            // Right
            x_values[2] = position[0] + 1;
            y_values[2] = position[1];
            // Down-right
            x_values[3] = position[0] + 1;
            y_values[3] = position[1] - 1;
            // Down
            x_values[4] = position[0];
            y_values[4] = position[1] - 1;
            // Down-left
            x_values[5] = position[0] - 1;
            y_values[5] = position[1] - 1;
            // Left
            x_values[6] = position[0] - 1;
            y_values[6] = position[1];
            // Up-left
            x_values[7] = position[0] - 1;
            y_values[7] = position[1] + 1;

            for(int i = 0; i < 8; i++){
                // Out of bound case
                if (x_values[i] <= 0 || y_values[i] <= 0 || x_values[i] >= MAP_WIDTH - 1 || y_values[i] >= MAP_HEIGHT - 1)
                    distance[i] = MAX_DISTANCE;
                else
                    distance[i] = sqrt( (x_values[i] - goal_x) * (x_values[i] - goal_x) + (y_values[i] - goal_y) * (y_values[i] - goal_y));
            }
            // Ordering and extracting here
            for(int i = 0; i < 8; i++){
                distance_ordered[i] = distance[i];
            }
            bubblesort(distance_ordered, 8);

            // Look for the index in distance with distance_ordered[0]
            for(int j = 0; j < 8; j++){
                if(distance[j] == distance_ordered[0]){
                    position[0] = x_values[j];
                    position[1] = y_values[j];
                }
            }
            // Send new position to master 
            n = write(sockfd, position, sizeof(position));
            if (n < 0) { 
                perror("ERROR writing to socket");
                writeErrorLog(fdlogError, "[Drone0] Error in writing to socket", errno);
                exit(EXIT_FAILURE);
            }

            log_desired_position(position[0], position[1]);

            printf("[Drone0] Writing (%d, %d) to server...\n", position[0], position[1]);
            fflush(stdout);
            // Wait for response 
            n = recv(sockfd, &response, sizeof(response), MSG_WAITALL);
            if (n < 0) {
                perror("ERROR reading response from socket\n");
                writeErrorLog(fdlogError, "[Drone0] Error in reading from socket", errno);
                exit(EXIT_FAILURE);
            }
            printf("[Drone0] response %d from server\n", response);

            // If response is collision, then update timeout and check condition on it
            if (response == MASTER_COL) {
                writeInfoLog(fdlogInfo, "[Drone0] Master answered permission denied");
                timeout++;
                position[0] = old_position[0];
                position[1] = old_position[1];
                // Wait before moving again
                msleep(TIMESTEP);
                if (timeout == DRONE_TIMEOUT) {
                    // If drone has collided "timeout" times, then change goal
                    goal_x = getRandomX();
                    goal_y = getRandomY();
                }
            } else if (response == MASTER_OK) {
                old_position[0] = position[0];
                old_position[1] = position[1];
                // No collision found
                writeInfoLog(fdlogInfo, "[Drone0] Master allows motion");
                // Wait before moving again
                msleep(TIMESTEP);
            } else {
                // Invalid response
                perror("Response not valid");
                exit(EXIT_FAILURE);
            }
            if (goal_x == position[0] && goal_y == position[1]) {
                // If on goal, find new goal
                goal_x = getRandomX();
                goal_y = getRandomY();
            }
        }
        // REFUELLING WITH MICROSLEEPS
        for (int k = 0; k < 30; k++) {
            // Landing for refuelling
            n = write(sockfd, position, sizeof(position));
            if (n < 0) { 
                perror("ERROR writing to socket");
                writeErrorLog(fdlogError, "[Drone0] Error in writing to socket", errno);
                exit(EXIT_FAILURE);
            }
            // Wastes responses from server while refuelling
            n = recv(sockfd, &response, sizeof(response), MSG_WAITALL);
            if (n < 0) {
                perror("ERROR reading response from socket\n");
                writeErrorLog(fdlogError, "[Drone0] Error in reading from socket", errno);
                exit(EXIT_FAILURE);
            }
            // Refuelling
            printf("[Drone0] refuelling\n");
            fflush(stdout);
            msleep(TIMESTEP);
        }
        writeInfoLog(fdlogInfo, "[Drone0] Fuel level ok");
    }
}