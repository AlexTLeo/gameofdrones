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
