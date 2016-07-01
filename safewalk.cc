/*
* To compile:
*
* g++ -c communicate.c
*
* g++ -o safewalk `pkg-config --cflags playerc++` safewalk.cc `pkg-config --libs playerc++` -lnsl communicate.o
*
*/
#include <libplayerc++/playerc++.h>  
#include <iostream>                  
#include "args.h"                    
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include "communicate.h"

#define RAYS 32                      

using namespace std;

int port;

/****************************************************************/

int timer (double reset){
   clock_t start;
   double duration = reset; 

   start = 0; //reset start
   start = clock();

   duration = ( 350*clock() - start ) / (double) CLOCKS_PER_SEC;

   //cout<<"current time: "<< duration <<'\n'; // Check if it works
   
   return duration;
}
/****************************************************************/
/* send command to the others */
void getCurrentLocation(int bfd, double x, double y)
{ // int x = 99;

  // create a message
  int nbytes = 0;
  char *msg = NULL;
  char *char_port = NULL;
  char *char_x = NULL;
  char *char_y = NULL;

  msg = (char *) malloc(60*sizeof(char));
  char_port = (char *) malloc(10*sizeof(char));
  char_x = (char *) malloc(10*sizeof(char));
  char_y = (char *) malloc(10*sizeof(char));

  itoa((int)port, char_port); // port
  itoa((int)x, char_x);       // position X
  itoa((int)y, char_y);       // position Y

  strcpy(msg, char_port);  // port
  strcat(msg, ":");
  strcat(msg, " ");
  strcat(msg, "(");
  strcat(msg, char_x);     // position X
  strcat(msg, ",");
  strcat(msg, " ");
  strcat(msg, char_y);     // position Y
  strcat(msg, ")");    
  
  printf("sending message: %s\n", msg);
  nbytes = talk_to_all(bfd, msg, H);
}
/****************************************************************/
void s_cmd(int bfd, char *cmd)
{
   // create a message
	 int nbytes = 0;
	 nbytes = talk_to_all(bfd, cmd, H);
}
/****************************************************************/
/* send command to the others */ 
void send_cmd(int bfd){
   int x = 99;

   // create a message
   int nbytes = 0;
   char *msg = NULL;
   char *char_from = NULL;
   char *char_x = NULL;

   msg = (char *) malloc(60*sizeof(char));
   char_from = (char *) malloc(10*sizeof(char));
   char_x = (char *) malloc(10*sizeof(char));

   itoa((int)port, char_from);
   itoa((int)x, char_x);

   strcpy(msg, "C");
   strcat(msg, char_from);
   strcat(msg, "$");
   strcat(msg, char_x);
   strcat(msg, "!");

   printf("sending message: %s\n", msg);
   nbytes = talk_to_all(bfd, msg, H);
}
/****************************************************************/
void parse_msg(char *msg)
{
   char *ptr, *token;

   token = strtok(msg, "!");
   while (token != NULL) {
      if (token[0] = 'C') {
         printf("This is a C type of message\n");
      }
      ptr = strstr(token, "C");
      ptr++;
      printf("port number %d\n", atoi(ptr));
      ptr = strstr(token, "$");
      ptr++;
      printf("x position is %d\n", atoi(ptr));
      token = strtok(NULL, "!");
  }
}
/****************************************************************/
/* main function */
int main(int argc, char** argv){
  
   bool forLeader, greenRed, firstRound=true, secondRound=true, finalRound=true;
   double reset;
   int lfd, bfd;  //listen & broadcast 
   int timeTest = 0;
   int nbytes = 0;
   char msg[MAXBUF];
   char *setCmd = (char *) malloc(10*sizeof(char));

   if (argc < 2) { 
       printf("mytest port\n");
       exit(1);
     }
     port = atoi(argv[1]); // converts comm line param from string to int

   // we throw exceptions on creation if we fail
   try
   { 
		    using namespace PlayerCc;
		 
		    PlayerClient     robot(gHostname, port); 
		    Position2dProxy  pp(&robot, gIndex);
		    LaserProxy       lp(&robot, gIndex);
		
		    std::cout << robot << std::endl;

		    pp.SetMotorEnable (true);
		   
		    // check port input
		    if (port != 6665){
		        lfd = create_listen(PORT_H,H);      //follower listens
		        bfd = create_broadcast(PORT_R,H);   //follower broadcasts
		        forLeader = false;
		    }else{
		        lfd = create_listen (PORT_R,H);     //leader listens
		        bfd = create_broadcast (PORT_H,H);  //leader broadcasts
		        forLeader = true;
		    }
		 
		    // go into read-think-act loop
		    double newspeed = 0, newturnrate = 0;

		    // if port is for leader
		    if(forLeader){
		       cout << "Command? ";
		       cin >> *setCmd;
		          
		       if(strcmp(setCmd, "s")==0){
		          cout << "\nAll robots start safewalking." << endl;
		          s_cmd(bfd,setCmd);             // leader sends msg to followers
		          greenRed = true;
		          sleep(1);                      // pauses for one sec then loops again
		       }else{
		          cout << "Wrong command. Try again." << endl;
		          exit(1); 
		       } // end else
		    } // end of if
		    	    
		    // if port is for follower
		    if(!forLeader){
		       while(1){
		          nbytes =listen_to_robot(lfd, msg);  //follower is listening
		          if(nbytes == 0) continue;
		          cout << "\nFollower is listening." << endl;
		          //sleep(2);
		             if (strcmp(msg, "s")==0){
		             cout << "Port " << port <<": " << "received msg: " << msg << endl;
		             greenRed = true;
		             break;
		             } //for if
		       } //for while
		    } //closing bracket for if(forLeader)
		    
		    while(1){
		       do {
		       
								    if((firstRound) || (secondRound)){
								    
										  			double newspeed = 0, newturnrate = 0;

										  			// this blocks until new data comes; 10Hz by default
										  			robot.Read();

										  			double minR = lp.GetMinRight();
										  			double minL = lp.GetMinLeft();
										 
										  			double l = (1e5*minR)/500-100;
										  			double r = (1e5*minL)/500-100;

										  			if (l > 100)
										    			 l = 100;
										  			if (r > 100)
										    			 r = 100;

										  			newspeed = (r+l)/1e3;

										  			newturnrate = (r-l);
										  			newturnrate = limit(newturnrate, -40.0, 40.0);
										  			newturnrate = dtor(newturnrate);

										  			// write commands to robot
										  			pp.SetSpeed(newspeed, newturnrate);
										  }
										 
										  //Followers constantly listening
												if(!forLeader)
												{
												 		nbytes = listen_to_robot(lfd, msg);
												 		if(strcmp(msg, "h")==0 ){  //h is halt 
													  			pp.SetSpeed(0,0);
													     cout << "Port " << port <<": " << "received msg: " << msg << endl;
													     if(!firstRound)
													        finalRound = false; 
													     greenRed = false;
													     			
												 		}//end if
												}// end if
										   
										  // Once 20 secs pass, greenRed set to false
										 	if(forLeader){ 
										 	   if(timeTest == 0){
										        if (20 == timer(reset)){
										           greenRed = false;
										           reset=0;
										           timeTest++;}
										     }else{
										        if (40 == timer(reset)){
										           greenRed = false;
										        }
										     }
										   }//end for Leader
								  } while (greenRed); 
								  
								  // STILL IN WHILE(1)
								  
								  // After 1st safewalk
								  if((forLeader) && (firstRound)){
								     cout << "\n20 Seconds have passed. Current Locations are: " << endl;
								     s_cmd(bfd, strcpy(setCmd, "h")); // halt
								     pp.SetSpeed(0.0,0.0);
								     greenRed = true;
								     sleep(1);
								  }
			       else if((forLeader) && (secondRound)){
				         cout << "\n20 Seconds passed. Mission Accomplished.\n" << endl;
				         s_cmd(bfd, strcpy(setCmd, "h"));  // halt
				         pp.SetSpeed(0.0,0.0);
				         secondRound = false;
				         finalRound  = false;
				         sleep(1);
			       }
								  if ((!forLeader) && (firstRound)){
								     pp.SetSpeed(0.0,0.0);
			          getCurrentLocation(bfd, pp.GetXPos(), pp.GetYPos());
								  }
								  
										if((forLeader) && (firstRound)){
													
													sleep(1);
													cout <<"\n"<< port << ": " << "(" << (int)pp.GetXPos() //print leader first
													     << ", " << (int)pp.GetYPos() << ")";
													
													for(int j=0; j<5; j++){
													   nbytes = listen_to_robot(lfd, msg);
													   cout <<  "    " << msg;
													}
													sleep(2);
													cout << "\n\nAll robots resume safewalking." << endl;
													s_cmd(bfd, strcpy(setCmd, "g"));  // go again
													firstRound = false;
												}// end if
								  
								  			// Listening to leader
													if((!forLeader) && (firstRound)){
																while (1) {
																			nbytes = listen_to_robot(lfd, msg);
																			if (nbytes == 0) continue;
																			if(strcmp(msg, "g")==0) {
																						cout << "Port " << port <<": " << "received msg: " << msg << endl;
																						firstRound = false;
																						greenRed = true; 
																						break;
																		}
																} 
													}
													
		           if ((forLeader) && (!finalRound)){
             			pp.SetSpeed(0.0,0.0);
			             return 0;
		           }
		           else if((!forLeader) && (!finalRound)){
			             pp.SetSpeed(0.0,0.0);
			             return 0;
		           }
     } // end of while(1)  
		 } // end of try
		 catch (PlayerCc::PlayerError & e){
		   std::cerr << e << std::endl;
		   return -1;
		 }
}

