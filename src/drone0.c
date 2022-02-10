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

char* string1;
char string2[30];
char* string3;
char string4[30];
char* string5;
int fdlogInfo;

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

int main(int argc, char *argv[]) {
    // Get starting position
    int position[2] = {START0[0], START0[1]};
    printf("Drone0: Initial position (%d, %d) \n", position[0], position[1]);
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
        exit(EXIT_FAILURE);
    }
    // Socket connection done
    writeInfoLog(fdlogInfo, "[Drone0] Successfully connected");

    // Max number of steps in the same direction
    const int same_direction = 5;
    // Timeout counter
    int timeout = 0;
    // Initialize response variable
    int response = 1;
    // Store old position in case of collisions
    int old_position[2];
    // Intializes random number generator
    srand(mix(clock(), time(NULL), getpid()));
    while (1) {
        for (int i = 0; i < STEPS; i++) {
            // Store old posiiton
            old_position[0] = position[0];
            old_position[1] = position[1];
            // Reset timeout counter
            timeout = 0;
            // Choose new direction
            int direction = rand() % 8;
            for (int j = 0; j < same_direction; j++) {
                // Update position
                if (direction == 0) {
                    // Up 
                    printf("Up ");
                    position[1]++;
                } else if (direction == 1) {
                    // Up - right
                    printf("Up - right ");
                    position[0]++;
                    position[1]++;
                } else if (direction == 2) {
                    // Right
                    printf("Right ");
                    position[0]++;
                } else if (direction == 3) {
                    // Down -right
                    printf("Down - right ");
                    position[0]++;
                    position[1]--;
                } else if (direction == 4) {
                    // Down
                    printf("Down ");
                    position[1]--;
                } else if (direction == 5) {
                    // Down - left
                    printf("Down - left ");
                    position[0]--;
                    position[1]--;
                } else if (direction == 6) {
                    printf("Left ");
                    // Left
                    position[0]--;
                } else if (direction == 7) {
                    // Up - left
                    printf("Up - left ");
                    position[0]--;
                    position[1]++;
                }
                // Send new position to master 
                n = write(sockfd, position, sizeof(position));
                if (n < 0) { 
                    perror("ERROR writing to socket");
                    exit(EXIT_FAILURE);
                }

                log_desired_position(position[0], position[1]);
                
                
                printf("Drone0: Writing (%d, %d) to server...\n", position[0], position[1]);
                fflush(stdout);
                // Wait for response 
                n = recv(sockfd, &response, sizeof(response), MSG_WAITALL);
                if (n < 0) {
                    perror("ERROR reading response from socket\n");
                    exit(EXIT_FAILURE);
                }
                printf("Drone0: response %d from server\n", response);
                    
                // If response is collision, then update timeout and check condition on it
                if (response == MASTER_COL) {
                    writeInfoLog(fdlogInfo, "[Drone0] Master answered permission denied");
                    timeout++;
                    position[0] = old_position[0];
                    position[1] = old_position[1];
                    if (timeout == DRONE_TIMEOUT) {
                        // If drone has collided "timeout" times, then change direction
                        break;
                    }
                } else if (response == MASTER_OK) {
                    // No collision found
                    writeInfoLog(fdlogInfo, "[Drone0] Master allows motion");
                    continue;
                } else {
                    // Invalid response
                    perror("Response not valid");
                    exit(EXIT_FAILURE);
                }
            }
            // Wait before moving again
            msleep(TIMESTEP);
        }
        // Landing for refuelling
        writeInfoLog(fdlogInfo, "[Drone0] Landing for refuelling...");
        n = write(sockfd, position, sizeof(position));
        if (n < 0) { 
            perror("ERROR writing to socket");
            exit(EXIT_FAILURE);
        }
        // Refuelling
        printf("Drone0: refuelling\n");
        fflush(stdout);
        sleep(3);
        writeInfoLog(fdlogInfo, "[Drone0] Fuel level ok");
    }
    printf("Drone0: Landing in (%d, %d)...\n", position[0], position[1]);
    fflush(stdout); 
    return 0;
}