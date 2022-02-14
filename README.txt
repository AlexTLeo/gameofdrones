# gameofdrones
# GROUP 5: Master

- Group 0: Michele Pestarino + Federico Sacco
- Group 1: Elea Papin + Paul Raingeard
- Group 2: Ebru Baglan + Tachadol Suthisomboon + Yusuke Kido
- Group 3: Antonio Bucciero + Marco Limone
- Group 4: Simone Contorno + Gabriele Russo
- Group 5: Alex Thanaphon Leonardi

Remember to set sockopt to reuse address port

Drones should generate trajectory in the form of:
1) number of total steps to take
2) random direction
Directions: 0 (up), 1 (top-right), 2 (right), ... , 7 (top-left)

See values.h for communication messages specifications

# TIMELINE
## 07/01
- simple client/server communication
## 14/01
- drone: able to generate trajectory (path + direction, see README), send next step to master, wait for master response before moving, if not start timeout (see values.h)
- master: receive all movements, check for collisions, respond to all drones 1 by 1 sequentially, draw map
## 21/01
- drone: able to manage its own power/fuel, sends master "out of fuel" message
- master: mark drones as landed or not
## 15/02
- drone: implement refueling (arbitrarily decide refuel time) by sending master its CURRENT coordinates
- master: reduce log spam
- master: if reading a drone's current coordinates being set, assume it is refueling and display it
- master: add visualization of already explored area
- master: fix collision issue when drone is beyond x = 40 y = 40
- drone: add logging# gameofdrones

- Group 0: Michele Pestarino + Federico Sacco
- Group 1: Elea Papin + Paul Raingeard
- Group 2: Ebru Baglan + Tachadol Suthisomboon + Yusuke Kido
- Group 3: Antonio Bucciero + Marco Limone
- Group 4: Simone Contorno + Gabriele Russo
- Group 5: Alex Thanaphon Leonardi

Remember to set sockopt to reuse address port

Drones should generate trajectory in the form of:
1) number of total steps to take
2) random direction
Directions: 0 (up), 1 (top-right), 2 (right), ... , 7 (top-left)

See values.h for communication messages specifications

# TIMELINE
## 07/01
- simple client/server communication
## 14/01
- drone: able to generate trajectory (path + direction, see README), send next step to master, wait for master response before moving, if not start timeout (see values.h)
- master: receive all movements, check for collisions, respond to all drones 1 by 1 sequentially, draw map
## 21/01
- drone: able to manage its own power/fuel, sends master "out of fuel" message
- master: mark drones as landed or not
## 15/02
- drone: implement refueling (arbitrarily decide refuel time) by sending master its CURRENT coordinates
- master: reduce log spam
- master: if reading a drone's current coordinates being set, assume it is refueling and display it
- master: add visualization of already explored area
- master: fix collision issue when drone is beyond x = 40 y = 40
- drone: add logging

# GROUP 0
-- Project description --
Drone0, made by group FeMi (Michele Pestarino S4672032, Federico Sacco S4672010).
The goal of our drone is to move randomly in the map without knowing anything about it, accordingly to the responses given by the master process.

-- Driving Idea --
Our drone after having established a socket connection with the master, will move for a certain number of STEPS, by choosing a new direction that minimize the distance with respect to the defined goal (found randomly).
If the drone gets 3 "detected collisions" in a row from the master, then it will choose another goal.
When the drone runs out of fuel, it will signal to the master that it is refuelling by sending to the main process its current position.
Every step is done at a rate of 5 steps/second, every refuelling lasts 3 seconds.
This is done, possibly, forever.
In order to compile the code run 'gcc drone0.c .o drone0 -lm'

# GROUP 1
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

# GROUP 2
<General information on the authors of this assignment>
	The name of the meta group is GoDr. The name of the small group is TTI composed of Ebru(5240270) and Tachadol(5240225) and Yusuke(5239225).

<What we did in this assignment>
	Our meta group implemented five drones flying and communicating with each other by using sockets. Our small group TTI implemented one of them named as "drone2" written in drone2.c. Drone2 plays a role of a client and send requests toward master which plays a role of a server.

<The contents of the package>
	This package is composed of drone2.c and readme_group2.txt.

<our drone's movement>
	 Our drone (drone2) can move inside of the field, avoiding collisions with other drones and not going out of the field. The fuel of it is limited, so the drone should refuel the energy when the fuel runs out. Until that charges the full of energy, the drone stays at the same position. When it finishes the charge, it will fly again. It repeats flying and charging.

1) flying
	a) creating random paths
		Our drone can move (fly) step by step in 8 directions: Up, Down, Left, Right, Up Left, Up Right, Down Left, Down Right. It can decide goal positions randomly and move along the shortest paths created in each processes with Dijkstra's Algorithm.  Random goal is created through reading dev/urandom, and for the Djikstra's algorithm, the number of computation is kept at 20 units. The connections between unit positions all are given equal cost that is why diagonal motions are also chosen mostly.

	b) collision avoidance
		Our drone does not move when another one is next to it. Firstly, our drone sends the request (via. socket) toward the master about the next step which it wants to take. Second of all, the master will decide whether it can move or not. If there is no drone around our drone, it can move. On the other hand, it will stay at the same position until other drones go away or reach the timeout (600 ms).

2) refueling
	Our drone's fuel is limited. It has 400 unit fuels originally and it loses 1 fuel every movement. When the fuel runs out, our drone should stay at the same position until it finishes refueling the energy to fly.


<Improvement in our drone>
1) Path planning algorithm (Dijkstra's Algorithm) and random goal position.
2) Log our drone in log files (info.log and errors.log).
3) ASCII art user interface with animation.


--------------------------------------------------------------------------------------------

# GROUP 3
<General information on the authors of this assignment>
	The name of the meta group is GoDr. The name of the small group is TTI composed of Ebru(5240270) and Tachadol(5240225) and Yusuke(5239225).

<What we did in this assignment>
	Our meta group implemented five drones flying and communicating with each other by using sockets. Our small group TTI implemented one of them named as "drone2" written in drone2.c. Drone2 plays a role of a client and send requests toward master which plays a role of a server.

<The contents of the package>
	This package is composed of drone2.c and readme_group2.txt.

<our drone's movement>
	 Our drone (drone2) can move inside of the field, avoiding collisions with other drones and not going out of the field. The fuel of it is limited, so the drone should refuel the energy when the fuel runs out. Until that charges the full of energy, the drone stays at the same position. When it finishes the charge, it will fly again. It repeats flying and charging.

1) flying
	a) creating random paths
		Our drone can move (fly) step by step in 8 directions: Up, Down, Left, Right, Up Left, Up Right, Down Left, Down Right. It can decide goal positions randomly and move along the shortest paths created in each processes with Dijkstra's Algorithm.  Random goal is created through reading dev/urandom, and for the Djikstra's algorithm, the number of computation is kept at 20 units. The connections between unit positions all are given equal cost that is why diagonal motions are also chosen mostly.

	b) collision avoidance
		Our drone does not move when another one is next to it. Firstly, our drone sends the request (via. socket) toward the master about the next step which it wants to take. Second of all, the master will decide whether it can move or not. If there is no drone around our drone, it can move. On the other hand, it will stay at the same position until other drones go away or reach the timeout (600 ms).

2) refueling
	Our drone's fuel is limited. It has 400 unit fuels originally and it loses 1 fuel every movement. When the fuel runs out, our drone should stay at the same position until it finishes refueling the energy to fly.


<Improvement in our drone>
1) Path planning algorithm (Dijkstra's Algorithm) and random goal position.
2) Log our drone in log files (info.log and errors.log).
3) ASCII art user interface with animation.


--------------------------------------------------------------------------------------------

# GROUP 4
