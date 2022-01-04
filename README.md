# gameofdrones

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
**07/01** 
- simple client/server communication
<br>
**14/01**
- drone: able to generate trajectory (path + direction, see README), send next step to master, wait for master response before moving, if not start timeout (see values.h)
- master: receive all movements, check for collisions, respond to all drones 1 by 1 sequentially, draw map
<br>
**21/01**
- drone: able to manage its own power/fuel, sends master "out of fuel" message
- master: mark drones as landed or not
<br>
From 14/01 to 21/01 is a lot of time for objectively really simple additions, so we might start working on improvements in that time period
