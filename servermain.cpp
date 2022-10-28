#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

/* You will to add includes here */
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cerrno>
#include <sys/time.h>
// Included to get the support library
#include <calcLib.h>

#include "protocol.h"


using namespace std;
/* Needs to be global, to be rechable by callback and main */
int loopCount=0;
int terminate=0;


/* Call back function, will be called when the SIGALRM is raised when the timer expires. */
void checkJobbList(int signum){
  // As anybody can call the handler, its good coding to check the signal number that called it.

  printf("Let me be, I want to sleep, loopCount = %d.\n", loopCount);

  if(loopCount>20){
    printf("I had enough.\n");
    terminate=1;
  }
  
  return;
}





int main(int argc, char *argv[]){
  
  /* Do more magic */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  int port=atoi(Destport);
  printf("Host %s, and Port %d.\n",Desthost,port);
  
  int opt = true;
  int master_socket, addrlen, new_socket, client_socket[5], max_clients = 5, activity, i, valread, sd;
  int max_sd;
  struct sockaddr_in address;
  
  char buffer[1025];
  fd_set readfds;
  
  for(int i = 0;i<max_clients;i++){
  	client_socket[i] = 0;
  }
  
  if((master_socket = socket(AF_INET, SOCK_DGRAM, 0)) == 0){
  	printf("Failed to create socket\n");
  	exit(-1);
  }
  
  if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt))<0){
  	printf("Failed to change socket options\n");
  	exit(-1);
  }
  
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  
  if(bind(master_socket, (struct sockaddr*)&address, sizeof(address))< 0){
  	printf("Failed to bind\n");
  	exit(-1);
  }
  else printf("Socket bound\n");
 
  
  addrlen = sizeof(address);
  printf("?\n");
  while(true){
  	
  	FD_ZERO(&readfds);
  	
  	FD_SET(master_socket, &readfds);
  	max_sd = master_socket;
  	
  	for(int i = 0;i< max_clients;i++){
  		sd = client_socket[i];
  		
  		if(sd>0){
  			FD_SET(sd, &readfds);
  		}
  		
  		if(sd > max_sd){
  			max_sd = sd;
  		}
  	
  	}
  	
  	activity = select(max_sd +1, &readfds, NULL, NULL, NULL);
  	
  	if((activity < 0) && (errno!=EINTR)){
  		printf("Select error\n");
  	 }
  	 
  	 if(FD_ISSET(master_socket, &readfds)){
  	 	if((new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen))<0){
  	 		printf("Failed accept\n");
  	 	}
  	 	
  	 	//Recv initial msg?
  	 	
  	 	
  	 	for(int i=0;i<max_clients;i++){
  	 		if(client_socket[i] == 0){
  	 			client_socket[i] = new_socket;
  	 		}
  	 	}
  	 }
  	 
  	 for(int i = 0;i<max_clients;i++){
  	 
  	 	sd = client_socket[i];
  	 }
  
  }
  	
		
  /* 
     Prepare to setup a reoccurring event every 10s. If it_interval, or it_value is omitted, it will be a single alarm 10s after it has been set. 
  */
  struct itimerval alarmTime;
  alarmTime.it_interval.tv_sec=10;
  alarmTime.it_interval.tv_usec=10;
  alarmTime.it_value.tv_sec=10;
  alarmTime.it_value.tv_usec=10;

  /* Regiter a callback function, associated with the SIGALRM signal, which will be raised when the alarm goes of */
  signal(SIGALRM, checkJobbList);
  setitimer(ITIMER_REAL,&alarmTime,NULL); // Start/register the alarm. 

#ifdef DEBUG
  printf("DEBUGGER LINE ");
#endif
  
  
  while(terminate==0){
    printf("This is the main loop, %d time.\n",loopCount);
    sleep(1);
    loopCount++;
  }

  printf("done.\n");
  return(0);


  
}
