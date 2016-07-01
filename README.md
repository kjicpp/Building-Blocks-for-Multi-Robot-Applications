# Building-Blocks-for-Multi-Robot-Applications

#Description

   The purpose of this project is to create the basic building blocks for  
   future multi-robot applications. Specifically, to implement the wander & 
   avoid_obstacle behaviors and to test communication among 6 robots via the
   program safewalk.cc.
   
   For the simulation, 6 robots are setup inside the multi.world & multi.cfg files. 
   They are equipped with laser sensors and have been assigned ports to enable
   listening and broadcast communication with each other.
   
   The following scenario has been implemented: Among all 6 robots, there's one
   leader robot (assigned port 6665) and 5 follower robots (assigned ports 6666-6670).
   Initially, all robots remain motionless. Once the leader receives a message from
   the operator, the leader sends commands to followers to begin safewalking. The
   safewalking will continue for 20 seconds before the leader sends a command to the
   followers to stop current behavior. Once the followers stop, they'll then send 
   back their current location (x,y) to the leader robot. The leader robot then 
   displays the location for each robot with its id. The leader then sends a 
   "resume" message to the followers. When the followers receive the message, they'll
   resume safewalking. After another 20 seconds, the leader sends an "exit" message
   to the followers. All robots then stop and exit their control programs simultaneously.
   

How to compile "communicate.h," "communicate.c" and "safewalk.cc":

[1] g++ -c communicate.c

[2] g++ -o safewalk `pkg-config --cflags playerc++` safewalk.cc `pkg-config --libs playerc++` -lnsl communicate.o

How to run the project:

[1] Place all files inside of "worlds" directory player stage directory
[2] Open a terminal for stage and a terminal for each robot
[3] Run "player multi.cfg" to launch stage simulator on first terminal
[4] Run "./safewalk 6666", "./safewalk 6667", "./safewalk 6668",
    "./safewalk 6669", and "./safewalk 6670" in each terminal
    subsequently after the second terminal
[5] For the leader robot terminal i.e. the second terminal, run
    "./safewalk 6665"
[6] You'll then see the "Command?" prompt on screen
[7] Enter "s" and the scenario described above will run sucessfully     




