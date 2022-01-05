#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "drone1_com.h"

int main(int argc, char *argv[])
{
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    int sock;
    if (create_and_connect_to_server(&sock) == 1){
        printf("connect error\n");
    }
    int response = send_message(sock,x,y);
    printf("%i\n", response);
    if (response == 2){
        printf("socket error for send\n");
    }

    //struct CoordinatePair coords = get_coords(sock);
       
    return 0;
}