/*
  -log file write in same files as master -> Yusuke | [Drone2]: xxxxxxxxxxxxxxxxxxxxxxx log("XXXXXXXXXXXXXXXX")
  -create path planning -> Ebru rand (0, 80) for x and y  (greedy search, A* star)
  -Print/visulize our drone status -> Tachadol
   ^
 < D >
   v  [----Battery-----]

  Update Fri. 10PM

  Sun. night 13/02/22
  *-.sh file + put some comments
*/
#include "../include/values.h"
#include "../include/utils.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <fcntl.h>

#define GROUPNUM 2
#define SA struct sockaddr
#define MINSTEP 3
#define MAXSTEP 3 //The real maxstep is maxstep + minstep


int x, y, n, coord[2], coord2[2], nCost, finalPath[20], dir[20], newCoord[20], finalPosition[40];
int signX = 1, signY = 1;


void log_dr2(int mode, char* string, int errno){

  int fd_log;

  //char exp_dr2[] = string;
  char con_dr2[80] = "[Drone 2]:";
  strcat(con_dr2, string);

  switch(mode)
  {


    case 0:
    fd_log = openInfoLog();
    writeInfoLog(fd_log, con_dr2);
    closeLog(fd_log);
    break;

    case 1: // for error
    fd_log = openErrorLog();
    writeErrorLog(fd_log, con_dr2, errno);
    closeLog(fd_log);
    break;
  }

}

int minDistance(int dist[], bool sptSet[])
{
    // This function is a part of Djikstra's algoritm.
    // Is used to find the minimum distance between two nodes.
    int min = INT_MAX, min_index;
    for (int v = 0; v < n; v++)
        if (sptSet[v] == false && dist[v] <= min)
            min = dist[v], min_index = v;
    return min_index;
}

void findPath(int dist[], int graph[n][n])
{
    // This function is a part of Djikstra's algoritm.
    // Is used to find the path for the minimum cost.
    // Normally Djikstra's algorithm only solves for
    // the minimum cost, but checking the costs of all
    // the nodes, and checking the neighbours' costs give
    // us the minimum cost path.

    int i, neighbour[n], a = 0, aux[20];
    // memset is used to set all integer array elements to zero
    memset(neighbour, 0, sizeof neighbour);
    // the first element of the path is the last node
    // x and y are the distance difference between Init and Goal values.
    finalPath[0] = (x+1)*(y+1)-1;
    int finalCost = dist[n-1];
    int finalNode = n-1;
    int b = 0;
    // the while loop starts from the last node and searches for neighbours
    // for one less cost. Using this way, all the path is created.
    while (finalCost)
    {
        for (i = 0; i < n; i++)
        {
            if (graph[finalNode][i] && dist[i] == finalCost - 1)
            {
                neighbour[a] = i, a++;
            }
        }
        finalNode = neighbour[b];
        finalCost--;
        b++;
        a = b;
    }

    // path is taken into global array finalPath
    for (i = 0; i < dist[n - 1]; i++)
        finalPath[i + 1] = neighbour[i];

    // the last element is taken as node 0
    finalPath[dist[n-1]] = 0;

    // starting from last and ending at the first node is reversed here.
    // now we will start from node 0 and end at the last node.
    for (i = 0; i < dist[n - 1] + 1; i++)
        aux[dist[n - 1] - i] = finalPath[i];

    for (i = 0; i < dist[n - 1] + 1; i++)
        finalPath[i] = aux[i];
}

void dijkstra(int graph[n][n], int src)
{
    // Djikstra's algorithm works like this:
    // The connections between the nodes are assumed to be infinite cost (INT_MAX).
    // In our case connections exist between a node and neighbouring 8 nodes.
    // Even the diagonal connections cost 1, like the direct ones. After providing
    // this cost array to algorithm, the infinite cost connections are recalculated
    // and assigned their true cost values. The cost of the last node is the cost of
    // the shortest way.
    // Minimum cost calculation for nodes is taken from:
    // https://www.tutorialspoint.com/c-cplusplus-program-for-dijkstra-s-shortest-path-algorithm

    int dist[n];
    bool sptSet[n];
    for (int i = 0; i < n; i++)
        dist[i] = INT_MAX, sptSet[i] = false;
    dist[src] = 0;
    for (int count = 0; count < n - 1; count++)
    {
        int u = minDistance(dist, sptSet);
        sptSet[u] = true;
        for (int v = 0; v < n; v++)
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX && dist[u] + graph[u][v] < dist[v])
                dist[v] = dist[u] + graph[u][v];
    }

    // the cost of all nodes from node 0 is send to findPath function.
    findPath(dist, graph);
    // shortest way cost is assigned here
    nCost = dist[n - 1];
}

void pathPlanning()
{
    int i, j;
    // the cost values of the nodes are kept in n times n integer array
    int graph[n][n];
    // the elements of graph array are all set to zero.
    memset(graph, 0, sizeof graph);

    // Cost array is defined for the rest of this function.
    // left bottom corner connectivities are defined
    graph[0][1] = 1, graph[0][x + 1] = 1, graph[0][x + 2] = 1;

    // right bottom corner connectivities are defined
    graph[x][x - 1] = 1, graph[x][2 * x] = 1, graph[x][2 * x + 1] = 1;

    // top left corner connectivities are defined
    graph[n - x - 1][n - 2 * x - 2] = 1, graph[n - x - 1][n - 2 * x - 1] = 1, graph[n - x - 1][n - x] = 1;

    // top right corner connectivities are defined
    graph[n - 1][n - x - 3] = 1, graph[n - 1][n - x - 2] = 1, graph[n - 1][n - 2] = 1;

    // bottom edge connectivities are defined
    for (i = 1; i < x; i++)
    {
        graph[i][i - 1] = 1, graph[i][i + 1] = 1;
        graph[i][i + x] = 1;
        graph[i][i + x + 1] = 1, graph[i][i + x + 2] = 1;
    }

    // top edge connectivities are defined
    for (i = n - x; i < n - 1; i++)
    {
        graph[i][i - x - 2] = 1, graph[i][i - x - 1] = 1;
        graph[i][i - x] = 1;
        graph[i][i - 1] = 1, graph[i][i + 1] = 1;
    }

    // left edge connectivities are defined
    for (j = 1; j < y; j++)
    {
        graph[j * (x + 1)][(j - 1) * (x + 1)] = 1, graph[j * (x + 1)][(j - 1) * (x + 1) + 1] = 1;
        graph[j * (x + 1)][j * (x + 1) + 1] = 1;
        graph[j * (x + 1)][(j + 1) * (x + 1)] = 1, graph[j * (x + 1)][(j + 1) * (x + 1) + 1] = 1;
    }

    // right edge connectivities are defined
    for (j = 1; j < y; j++)
    {
        graph[(j + 1) * (x + 1) - 1][(j - 1) * (x + 1)] = 1, graph[(j + 1) * (x + 1) - 1][(j - 1) * (x + 1) + 1] = 1;
        graph[(j + 1) * (x + 1) - 1][j * (x + 1) - 1] = 1;
        graph[(j + 1) * (x + 1) - 1][(j + 1) * (x + 1)] = 1, graph[(j + 1) * (x + 1) - 1][(j + 1) * (x + 1) + 1] = 1;
    }

    // for middle elements
    for (j = 1; j < y; j++)
    {
        for (i = j * (x + 1) + 1; i < (j + 1) * (x + 1) - 1; i++)
        {
            graph[i][i - x - 2] = 1, graph[i][i - x - 1] = 1, graph[i][i - x] = 1;
            graph[i][i - 1] = 1, graph[i][i + 1] = 1;
            graph[i][i + x] = 1, graph[i][i + x + 1] = 1, graph[i][i + x + 2] = 1;
        }
    }

    // then this cost array is sent to the Djikstra's algorithm.
    dijkstra(graph, 0);

}

void randPos(int xLim, int yLim, int array[])
{
    // x limit and y limit for random numbers are taken, not to randomize huge numbers
    // a global array is used for the transfer of the values from function to main()
    // because in C there are no other way to return multiple values
    int i, lims[2];
    lims[0] = xLim;
    lims[1] = yLim;
    unsigned char buffer[4];
    char str1[20], str2[20];

    // randomizing starts here
    // Why urandom should be chosen over random:
    // https://unix.stackexchange.com/questions/324209/when-to-use-dev-random-vs-dev-urandom
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, buffer, 4);
    close(fd);

    for (i = 0; i < 2; ++i)
    {
        // Concatenating two integers
        // https://www.geeksforgeeks.org/how-to-concatenate-two-integer-values-into-one/
        sprintf(str1, "%d", (int)buffer[2 * i]);
        sprintf(str2, "%d", (int)buffer[2 * i + 1]);
        strcat(str1, str2);
        array[i] = atoi(str1) % lims[i];
    }
}

void obtainDirections()
{
    // The relationship between adjacent elements of shortest
    // path is checked to find the direction to follow.
    // Direction numbers are defined as follows:
    // 0 = up,  1 = upper right, 2 = right, 3 = lower right,
    // 4 = down 5 = lower left,  6 = left,  7 = upper left.
    int i, diff;
    dir[0] = 0;
    for (i = 1; i < nCost + 1; i++)
    {
        diff = finalPath[i] - finalPath[i - 1];
        if (signX == 1 && signY == 1)
        {
            if (diff == x + 1)
                dir[i] = 0;
            else if (diff == x + 2)
                dir[i] = 1;
            else if (diff == 1)
                dir[i] = 2;
            else
                printf("Something smells fishy.\n");
        }
        else if (signX == 1 && signY == -1)
        {
            if (diff == x + 1)
                dir[i] = 4;
            else if (diff == x + 2)
                dir[i] = 3;
            else if (diff == 1)
                dir[i] = 2;
            else
                printf("Something smells fishy.\n");
        }

        else if (signX == -1 && signY == 1)
        {
            if (diff == x + 1)
                dir[i] = 0;
            else if (diff == x + 2)
                dir[i] = 7;
            else if (diff == 1)
                dir[i] = 6;
            else
                printf("Something smells fishy.\n");
        }

        else if (signX == -1 && signY == -1)
        {
            if (diff == x + 1)
                dir[i] = 4;
            else if (diff == x + 2)
                dir[i] = 5;
            else if (diff == 1)
                dir[i] = 6;
            else
                printf("Something smells fishy.\n");
        }
    }
}

void newPos(int xP, int yP, int newDir)
{
    // The direction numbers are changed into increase/decrease
    // in x and y coordinates. Because at the end of the day, what we
    // send to master is absolute positions, that we want to move to.

    switch (newDir)
    {
    case 0: /* up */
        yP = yP + 1; break;
    case 1: // upperright
        xP = xP + 1, yP = yP + 1; break;
    case 2: // right
        xP = xP + 1; break;
    case 3: // bottom right
        yP = yP - 1, xP = xP + 1; break;
    case 4: // bottom
        yP = yP - 1; break;
    case 5: // bottom left
        xP = xP - 1, yP = yP - 1; break;
    case 6: // left
        xP = xP - 1; break;
    case 7: // upper left
        yP = yP + 1, xP = xP - 1; break;
    default:
        printf("random: something unexpected happened.\n");
        break;
    }
    newCoord[0] = xP, newCoord[1] = yP;
}

int eburu_pos(int xInit, int yInit){
    // xLim, yLim = Map limits. xInit, yInit = Current pos.
    // xDiffLim, yDiffLim = Defined because of computational limitation of
    // Djikstra's algorithm. Later set to 20.
    int xLim = 80, yLim = 40, xGoal, yGoal, xDiffLim, yDiffLim, i;
    int xCurr, yCurr;
    signX = 1, signY = 1;

    // global arrays are initialized.
    coord2[0] = 0, coord2[1] = 0;

    // implementation of greedy algorith of Djikstra's require lots of memory space,
    // that is why the limitation between current and goal is kept within (36, 36)
    // but for initial purposes, 20 is also okay, can be increased later on.
    xDiffLim = 20, yDiffLim = 20;

    //  goal coordinates are assigned in coord2 global array.
    randPos(xDiffLim, yDiffLim, coord2);

    // if the distance between current and goal is smaller than 3, it is increased to lengthen the path.
    if (coord2[0] < 3)  coord2[0] = coord2[0] + 3;
    if (coord2[1] < 3)  coord2[1] = coord2[1] + 3;

    // if goal coordinates are out of map(>40 or >80), then they are subtracted from current newPos,
    // so the goal is behind us now.

    unsigned char rand_direction[4];
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, rand_direction, 4);
    close(fd);
    int direction[2];
    char str1[20], str2[20];
    for (i = 0; i < 2; ++i)
    {
        sprintf(str1, "%d", (int)rand_direction[2 * i]);
        sprintf(str2, "%d", (int)rand_direction[2 * i + 1]);
        strcat(str1, str2);
        direction[i] = atoi(str1) % 1;
    }

    if (direction[0] == 0){
      if (xInit + coord2[0] > xLim){
          signX = -1;
          if (xInit - coord2[0] >= 0)
              xGoal = xInit - coord2[0];
          else if (abs(xInit - coord2[0])>=3){
              xGoal = abs(xInit - coord2[0]);
              coord2[0] = xInit - xGoal;
          }
          else
          {
              xGoal = xInit - 3;
              coord2[0] = 3;
          }
      }
      else xGoal = xInit + coord2[0];
    }
  else {
    if (xInit - coord2[0] < 0){
        signX = 1;
        if (xInit - coord2[0] >= 0)
            xGoal = xInit - coord2[0];
        else if (abs(xInit - coord2[0])>=3){
            xGoal = abs(xInit - coord2[0]);
            coord2[0] = xInit - xGoal;
        }
        else
        {
            xGoal = xInit - 3;
            coord2[0] = 3;
        }
    }
    else xGoal = xInit - coord2[0];
  }
  if (direction[1] == 0){
    if (yInit + coord2[1] > yLim){
        signY = -1;
        if (yInit - coord2[1] >= 0)
            yGoal = yInit - coord2[1];
        else if (abs(yInit - coord2[1])>=3){
            yGoal = abs(yInit - coord2[1]);
            coord2[1] = yInit - yGoal;
        }
        else
        {
            yGoal = yInit - 3;
            coord2[1] = 3;
        }
    }
    else yGoal = yInit + coord2[1];
  }
  else{
    if (yInit - coord2[1] < 0){
        signY = 1;
        if (yInit - coord2[1] >= 0)
            yGoal = yInit - coord2[1];
        else if (abs(yInit - coord2[1])>=3){
            yGoal = abs(yInit - coord2[1]);
            coord2[1] = yInit - yGoal;
        }
        else
        {
            yGoal = yInit - 3;
            coord2[1] = 3;
        }
    }
    else yGoal = yInit - coord2[1];
  }

    // for finalPath planning function, global variables x and y are defined as well as node number.
    x = coord2[0], y = coord2[1], n = (x + 1) * (y + 1);

    // shortest path is obtained with node numbers inside finalPath global array
    pathPlanning();

    printf("Current =  (%d, %d)\n", xInit, yInit);
    printf("Goal    =  (%d, %d)\n\n", xGoal, yGoal);

    // obtain directions, 0 for up, 1 for upper right, 2 is for right... like in our original code.
    obtainDirections();

    // the required positions are obtained in for loop
    xCurr = xInit, yCurr = yInit;
    newCoord[0] = xInit, newCoord[1] = yInit;
    printf("Start\t(%d, %d)\n", xCurr,yCurr);
    for (i = 1; i < nCost+1; i++)
    {
        newPos(xCurr,yCurr, dir[i]);
        xCurr = newCoord[0], yCurr = newCoord[1];
        printf("Pos %d \t(%d, %d)\n", i, xCurr,yCurr);
        finalPosition[2*(i-1)] = xCurr;
        finalPosition[2*(i-1)+1] = yCurr;
    }
    printf("\nGoal was(%d, %d)\n\n", xGoal, yGoal);

}

void errorPrompt(int state,char error_msg[])
{
  if (state < 0){
    perror(error_msg);
    log_dr2(1, error_msg, 0);
    exit(EXIT_FAILURE);
  }
}


int sendAndReceive(int sockfd, int goTo [])
{
  int returnMSG;
  //msg2server = "Hello world from group 2";
  printf("Sending msg...... goTo %d, %d", goTo[0], goTo[1]);
  //write how many bytes that server should recieve -------
  // Sending co-ordiante to master
  if (write(sockfd, goTo, sizeof(int)*2) < 0 ){
    perror("sending failed...");
    exit(0);
  }
  printf("Send successfully\n");
  // Recieve the permission
  if (read(sockfd, &returnMSG, sizeof(int)*1) < 0 ){
    perror("reading failed...");
    exit(0);
  }
  printf("From Server : %d\n", returnMSG);

  if(returnMSG == MASTER_COL){
    printf("Rejected by Master (rejected code: %d), unable to move to %d, %d", returnMSG, goTo[0], goTo[0]);
    return 0;
  }
  else if (returnMSG == MASTER_OK){
      printf("Approved by Master (code: %d), moving to %d, %d", returnMSG, goTo[0], goTo[0]);
      return 1;
  }
}

// fuction to log the status of the drone (mode 0 for normal event and mode 1 for error)


int main()
{
  //setup the server connection
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;
  struct hostent *server;
  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  errorPrompt (sockfd, "socket creation failed...");
  log_dr2(0, "Socket successfully created" ,0);
  printf("Socket successfully created..\n");
  //add reuse socket addr and port
  errorPrompt (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)), "setsockopt");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  server = gethostbyname("localhost");
  if(server == NULL ){
    perror("ERROR, no such host");
    close(sockfd);
    exit(0);
  }
  memcpy((char *)server->h_addr, (char *)&servaddr.sin_addr.s_addr, server->h_length);
  servaddr.sin_port = htons(PORTNO+2);

  //init the timestep for loop using sleep
  struct timeval tv;
  tv.tv_sec = 0;
  tv.tv_usec = TIMESTEP*1000;

  // init the start of drone
  int xInit = START2[1], yInit = START2[0], i,j;

  int fuel = 500;

  errorPrompt(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)), "connection with the server failed...");
  log_dr2(0, "Connected to the server", 0);
  printf("connected to the server..\n");
  int moving_to[2];
  while(1){
    if (fuel < 20){
      for(int rest = 0; rest < 20; rest++){
        sendAndReceive(sockfd, moving_to);
        errorPrompt(select(1, NULL, NULL, NULL, &tv), "select failed...");
      }
      fuel = 500;
      //Send the same positon to master
    }
    //Generate the goal and find the path the reach that
    eburu_pos(xInit, yInit);
    for (j = 0; j<nCost ; j++)
    {
      int res_from_master =0, timeout_counter = 0;
      while (timeout_counter < DRONE_TIMEOUT && res_from_master == 0){
          errorPrompt(select(1, NULL, NULL, NULL, &tv), "select failed...");

          moving_to[0] = finalPosition[2*j];
          moving_to[1] = finalPosition[2*j+1];
          res_from_master = sendAndReceive(sockfd, moving_to);
          timeout_counter++;
      }
      if(res_from_master == 1){
        printf("\tgoTo: (%d, %d)\n", finalPosition[2*j], finalPosition[2*j+1]);
        fuel--;
      }
      else{
        printf("Unable to reach, going to find a new goal\n");
        break;
      }
    }
    xInit = newCoord[0], yInit = newCoord[1];



  //Using select insead of sleep for preciser timing ref: https://stackoverflow.com/questions/3125645/why-use-select-instead-of-sleep
  }
  errorPrompt(close(sockfd), "close failed...");
  printf("Close successfully..\n");
  return 0;
}


//
// void print_drone(int batt_per, int goal_x, int goal_y, int cur_x, int cur_y, int move_x, int move_y, float timestep){
//     system("clear");
//     printf(" _____                         ___  \n"
//             "|  __ \\                       |__ \\ \n"
//             "| |  | |_ __ ___  _ __   ___     ) |\n"
//             "| |  | | '__/ _ \\| '_ \\ / _ \\   / / \n"
//             "| |__| | | | (_) | | | |  __/  / /_ \n"
//             "|_____/|_|  \\___/|_| |_|\\___| |____|\n"
//             "                                    \n \n ");
//     fflush(stdout);
//     //https://manytools.org/hacker-tools/convert-images-to-ascii-art/go/ ascii art
//     printf ("                                                    |==================\n"
//             "     @@@@@@@@@                      @@@@@@@@@        |       Battery\n");
//     printf ("    @@        @@(                 @@       .@@       |      [  %d  }\n", batt_per);
//     printf ("  @&         #& .@.             #@  @(         @@    |==================\n"
//             " @&     #@@@&    ,@             @     @@@@      @@   |   Goal corodinate\n");
//     printf (" @(     @@@@@#   .@            ,@    @@@@@@     @@   |     [ %d, %d ] \n", goal_x, goal_y);
//     printf (" .@   @#   @@@@@@@#             @@@@@@@&   @@  /@    |==================\n"
//             "   @@*       @@@@@@@(         &@@@@@@@       /@(     |   Current position\n"
//             "      ,@@@@@@/.@@@@@@@@@@@@@@@@@@@@@ #@@@@@@.        |     [ %d, %d ] \n", cur_x, cur_y);
//     printf ("                #@@@@@@@@@@@@@@@@@                   |==================\n"
//             "                 @@@@@@@@@@@@@@@@@                   |      Moving to\n"
//             "                 @@@@@@@@@@@@@@@@@                   |     [ %d, %d ] \n", move_x, move_y);
//     printf ("                 @@@@@@@@@@@@@@@@@                   |==================\n"
//             "               /@@@@@@@@@@@@@@@@@@@                  |      Time step\n"
//             "    &@@.     @@@@@@@@@@@@@@@@@@@@@@@@&     /@@*      |        %.2f \n", timestep);
//     printf ("  @@ ,      @@@@@@(             @@@@@@@     .  @&    |==================\n"
//             " @@    @@@@@@@@  (@             @.  @@@@@@@@    @#\n"
//             " @/     @@@@.     @            /@     #@@@@     @@\n"
//             " (@         @@   @@             @#   @@         @.\n"
//             "   @@          #@*               3@.          @@  \n"
//             "     ,@@@@@@@@#                     @@@@@@@@@     \n");
//     fflush(stdout);
// }
//
//
// int print_drone_charge(){
//     int child_pid;
//     child_pid = fork();
//     if (child_pid == 0){
//       // parent process because return value non-zero.
//       system("clear");
//       while(1){
//         char symbol[22] = {60, 45, 45, 45, 60, 45, 45, 45, 60, 45, 45, 45, 60, 45, 45, 45, 60, 45, 45, 45, 60, 45};
//         for (int i = 1; i < 5; i++){
//           usleep(80000);
//           for (int j = 0; j < 21; j++){
//             symbol[j] = symbol[j+1];
//           }
//           if (i == 4){
//             symbol[21] = 60;
//           }
//           else {
//             symbol[21] = 45;
//           }
//           system("clear");
//           printf(" _____                         ___  \n"
//           "|  __ \\                       |__ \\ \n"
//           "| |  | |_ __ ___  _ __   ___     ) |\n"
//           "| |  | | '__/ _ \\| '_ \\ / _ \\   / / \n"
//           "| |__| | | | (_) | | | |  __/  / /_ \n"
//           "|_____/|_|  \\___/|_| |_|\\___| |____|\n"
//           "                                    \n \n ");
//
//           printf ("                                                  \n"
//           "     @@@@@@@@@                      @@@@@@@@@              \n");
//           printf ("    @@        @@(                 @@       .@@        \n");
//           printf ("  @&         #& .@.             #@  @(         @@ \n"
//           " @&     #@@@&    ,@             @     @@@@      @@\n"
//           " @(     @@@@@#   .@            ,@    @@@@@@     @@              @@      @@ \n"
//           " .@   @#   @@@@@@@#             @@@@@@@&   @@  /@              @@@@    @@@@\n"
//           "   @@*       @@@@@@@(         &@@@@@@@       /@(               @@@@    @@@@ \n"
//           "      ,@@@@@@/.@@@@@@@@@@@@@@@@@@@@@ #@@@@@@.               @@@@@@@@@@@@@@@@@ \n"
//           "                #@@@@@@@@@@@@@@@@@                            #@@@@@@@@@@@@@  \n"
//           "                 @@@@@@@@@@@@@@@@@                             @@@@@@@@@@@@  \n"
//           "                 @@@@@@@@@@@@@@@@@                              @@@@@@@@@@  \n");
//           printf("                 @@@@@@@@@@@@@@@@@%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c@@@@@*        @@@@    \n", symbol[0], symbol[1], symbol[2], symbol[3], symbol[4], symbol[5], symbol[6], symbol[7], symbol[8], symbol[9], symbol[10], symbol[11], symbol[12], symbol[13], symbol[14], symbol[15], symbol[16], symbol[17], symbol[18], symbol[19]);
//           printf("               /@@@@@@@@@@@@@@@@@@@                   %@@@@        @@@@\n"
//           "    &@@.     @@@@@@@@@@@@@@@@@@@@@@@@&     /@@*         @@@@       @@@@ \n"
//           "  @@ ,      @@@@@@(             @@@@@@@     .  @&       #@@@@     @@@@# \n"
//           " @@    @@@@@@@@  (@             @.  @@@@@@@@    @#        @@@@@@@@@@@ \n"
//           " @/     @@@@.     @            /@     #@@@@     @@\n"
//           " (@         @@   @@             @#   @@         @.           [ Charging ]\n"
//           "   @@          #@*               3@.          @@  \n"
//           "     ,@@@@@@@@#                     @@@@@@@@@     \n");
//           fflush(stdout);
//
//       }
//       //https://manytools.org/hacker-tools/convert-images-to-ascii-art/go/ ascii art
//
//
//     }
//   }
//     else{
//       return child_pid;
//     }
// }
