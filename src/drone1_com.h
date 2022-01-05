#ifndef DRONE1_COM_H_ 
#define DRONE1_COM_H_

int create_and_connect_to_server(int *sock);

int send_message(int sock,int x,int y);

struct CoordinatePair get_coords(int sock);

#endif // DRONE1_COM_H_
