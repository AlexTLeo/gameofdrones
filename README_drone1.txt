[DRONE 1][GENERAL DESCRIPTION]

authors:
  - Paul Raingeard de la Bletiere
  - Elea Papin
  If there is any problem while running this code, we can be contacted with the email adresses p.raingeard.37@gmail.com and elea.papin@eleves.ec-nantes.fr

usage: 
    - master : used as server to manage all the other drones.
    - drone1 : simulation of a drone giving random path to master.
    
description:
    
The drone1 program implements one drone that moves within a map. The drone does not know of the limits of the maps nor of the presence of other drones. Therfore, it must communicate with a master to get this information. The communication is done through sockets : the master is the server and the drone1 is a client. At every pass in the loop, the drone asks the master if he can move to a certain position. The master checks feasability and returns an answer. If no collision (with the walls or other drones) would result in the movement, the drone changes position. If not, it stays in place.

The drone chooses randomly the direction in which it is going to move. It can move in straight lines or diagonally. In order to dorce the drone to explore the map, one direction is set for several consecutive loops. If a movement is rejected by the master 3 consecutive times, the direction is changed since it can be interpreted as a collision with a wall and not with a drone. The drone also have a certain amount of fuel alloted at the beginning. Each pass in the loop results in the loss of one unit of fuel. When the tank is empty, the drone no longer tries to move; it sends its current position to the master and starts refueling. Once refueled, it goes back to normal operations.
    
 
