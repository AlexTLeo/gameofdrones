[GENERAL DESCRIPTION]

authors:
  - Paul Raingeard de la Bletiere
  - Elea Papin
  If there is any problem while running this code, we can be contacted with the email adresses p.raingeard.37@gmail.com and elea.papin@eleves.ec-nantes.fr

usage: 

  prerequisites : 
    - gcc compiler 

  to run the programs : 
    - first use install by giving it the path to extract your files : . install.sh WANTED_PATH
    - if no path is specified, everything is unzipped in the ./archive folder
    BE CAREFUL : no slash in the call to the scripts
    - then run using . run.sh
    - to uninstall use . uninstall.sh
    - once installed, you can call help.sh to get more information about the different processes

  scripts :

    - install.sh : unzip the archive and compile the executables.
    - run.sh : run the executables once they have been installed.
    - cleanup.sh : remove the executables. You need to run install.sh again to recompile. Also clean logs.
    - uninstall.sh : delete the added files and the unziped archive.
    - help.sh : shows this document and the description of the other processes if they have been installed

  processes : 

    - master : used as server to manage all the other drones.
    - drone1 : simuulation of a drone giving random path to master.
    
  description:
    
  Implements a communication between a server and a drone, that checks for collision between several drones.
    
  commands:

  The user just has to run the program to get a simulation.