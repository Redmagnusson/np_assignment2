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

	int sockfd;
	char buffer[1024];
	struct sockaddr_in servaddr, clientaddr;
	
	//Create socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		printf("Socket creation failed\n");
		
	}else printf("Socket created\n");
	
	socklen_t server_struct_length = sizeof(servaddr);
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&clientaddr, 0, sizeof(clientaddr));
	
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	
	//Bind socket
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
		printf("Socket bind failed\n");
		
	}else printf("Socket bound\n");
	
	struct calcMessage calc_message;
	struct calcProtocol calc_protocol;
	
	//Read msg
  if(recvfrom(sockfd, (struct calcProtocol*)&calc_message, sizeof(calcMessage), 0, 
  	(struct sockaddr*)&servaddr, &server_struct_length) < 0){
  		printf("Failed to read msg. Errno: %s\n", strerror(errno));
		exit(-1);
  
  }
  else printf("Client Responded: %d\n", ntohs(calc_message.type));
	
	//Check full message to be correct
	
	//TODO Timeout 10s
	
	//If lost client attempts to send msg, reject
	
	//Wrong ID = reject, track client IP, port, ID and assignment
		
		
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
