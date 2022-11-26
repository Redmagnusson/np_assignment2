#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

/* You will to add includes here */
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
// Included to get the support library
#include <calcLib.h>

#include "protocol.h"

#define DEBUG
using namespace std;
const int USERS = 5;
struct ClientInfo{
	struct sockaddr_in CLIENTINFO;
	struct calcProtocol PROTO;
	int LEN;
	int TIMER = -1;
};
/* Needs to be global, to be rechable by callback and main */
int loopCount=0;
int terminate=0;
struct ClientInfo CLIENTS[USERS];

/* Call back function, will be called when the SIGALRM is raised when the timer expires. */
void checkJobbList(int signum){
  // As anybody can call the handler, its good coding to check the signal number that called it.

  //printf("Let me be, I want to sleep, loopCount = %d.\n", loopCount);
  for(int i = 0;i<USERS;i++){
  	if(CLIENTS[i].TIMER != -1){
  	  	CLIENTS[i].TIMER++;
  	}
  	if(CLIENTS[i].TIMER >= 10){
  		printf("Client with ID %d dropped\n", ntohs(CLIENTS[i].PROTO.id));
  		CLIENTS[i].PROTO.id = -1;
  		CLIENTS[i].TIMER = -1;
  		
  	}
  }
  printf("We're running the timer loop\n");
  return;
}

void createAssignment(struct calcProtocol &proto){
	
	int inValue1, inValue2, inResult;
	float flValue1, flValue2, flResult;
	//Generate arith
	char* arith = randomType();
	#ifdef DEBUG
	printf("Arith: %s\n", arith);
	#endif
	//Calculate assignment
	if(strcmp(arith, "add") == 0){
		inValue1 = randomInt(); inValue2 = randomInt();
		printf("ASSIGNMENT: Add %d %d\n", inValue1, inValue2);
		inResult = inValue1 + inValue2;
		proto.inValue1 = htonl(inValue1);
		proto.inValue2 = htonl(inValue2);
		proto.inResult = htonl(inResult);
		proto.arith = htonl(1);
	}
	else if (strcmp(arith, "sub") == 0){
		inValue1 = randomInt(); inValue2 = randomInt();
		printf("ASSIGNMENT: Sub %d %d\n", inValue1, inValue2);
		inResult = inValue1 - inValue2;
		proto.inValue1 = htonl(inValue1);
		proto.inValue2 = htonl(inValue2);
		proto.inResult = htonl(inResult);
		proto.arith = htonl(2);
	}
		else if (strcmp(arith, "mul") == 0){
		inValue1 = randomInt(); inValue2 = randomInt();
		printf("ASSIGNMENT: Mul %d %d\n", inValue1, inValue2);
		inResult = inValue1 * inValue2;
		proto.inValue1 = htonl(inValue1);
		proto.inValue2 = htonl(inValue2);
		proto.inResult = htonl(inResult);
		proto.arith = htonl(3);
	}
		else if (strcmp(arith, "div") == 0){
		inValue1 = randomInt(); inValue2 = randomInt();
		//Make sure not to divide by 0
		while(inValue2 == 0){
			inValue2 = randomInt();
		}
		printf("ASSIGNMENT: Div %d %d\n", inValue1, inValue2);
		inResult = inValue1 / inValue2;
		proto.inValue1 = htonl(inValue1);
		proto.inValue2 = htonl(inValue2);
		proto.inResult = htonl(inResult);
		proto.arith = htonl(4);
	}
		else if (strcmp(arith, "fadd") == 0){
		flValue1 = randomFloat(); flValue2 = randomFloat();
		printf("ASSIGNMENT: fadd %8.8g %8.8g\n", flValue1, flValue2);
		flResult = flValue1 + flValue2;
		proto.flValue1 = flValue1;
		proto.flValue2 = flValue2;
		proto.flResult = flResult;
		proto.arith = htonl(5);
	}
		else if (strcmp(arith, "fsub") == 0){
		flValue1 = randomFloat(); flValue2 = randomFloat();
		printf("ASSIGNMENT: fsub %8.8g %8.8g\n", flValue1, flValue2);
		flResult = flValue1 - flValue2;
		proto.flValue1 = flValue1;
		proto.flValue2 = flValue2;
		proto.flResult = flResult;
		proto.arith = htonl(6);
	}
		else if (strcmp(arith, "fmul") == 0){
		flValue1 = randomFloat(); flValue2 = randomFloat();
		printf("ASSIGNMENT: fmul %8.8g %8.8g\n", flValue1, flValue2);
		flResult = flValue1 * flValue2;
		proto.flValue1 = flValue1;
		proto.flValue2 = flValue2;
		proto.flResult = flResult;
		proto.arith = htonl(7);
	}
		else if (strcmp(arith, "fdiv") == 0){
		flValue1 = randomFloat(); flValue2 = randomFloat();
		//Make sure not to divide by 0
		while(inValue2 == 0){
			inValue2 = randomFloat();
		}
		printf("ASSIGNMENT: fdiv %8.8g %8.8g\n", flValue1, flValue2);
		flResult = flValue1 / flValue2;
		proto.flValue1 = flValue1;
		proto.flValue2 = flValue2;
		proto.flResult = flResult;
		proto.arith = htonl(8);
	}
	//Finalize msg
	proto.type = ntohs(1);
	proto.major_version = ntohs(1);
	proto.minor_version = ntohs(0);
}

bool compareProtocols(struct calcProtocol &server, struct calcProtocol &client){

	if(2 == ntohs(client.type) && ntohs(server.major_version) == ntohs(client.major_version) && ntohs(server.minor_version) == ntohs(client.minor_version) && ntohl(server.id) == ntohl(client.id) && ntohl(server.arith) == ntohl(server.arith)){
		if(ntohl(server.arith) <=4){
			if(ntohl(server.inValue1) == ntohl(client.inValue1) && ntohl(server.inValue2) == ntohl(client.inValue2) && ntohl(server.inResult) == ntohl(client.inResult)){
			
				return true;
			}
		}
		else if(ntohl(server.arith) > 4){
		//Add floating point error check?
			if(server.flValue1 == client.flValue1 && server.flValue2 == client.flValue2 == server.flResult == client.flResult){
				return true;
			}
		}
		else return false;
	}
	else return false;
}

int CAP = 2000;
int main(int argc, char *argv[]){
  //Variables
	char* splits[CAP];
  char* p = strtok(argv[1], ":");
  int delimCounter = 0;
  char *Desthost;
  char *Destport;
  int port;
	int sockfd;
	int connfd;
	struct sockaddr_in client;
	char server_message[CAP];
	char client_message[CAP];
	char msg[CAP];
	int len = 0;
	float fv1, fv2, fresult;
	int iv1, iv2, iresult;
	int readSize;
	int nextID = 1;
	initCalcLib_seed(1000);
  //Get argv
  while(p != NULL){
  	//Look for the amount of ":" in argv to determine if ipv4 or ipv6
  	splits[delimCounter++] = p;
  	p = strtok(NULL, ":");
  }
  Destport = splits[--delimCounter];
  Desthost = splits[0];
  for(int i = 1;i<delimCounter;i++){
  	
  	sprintf(Desthost, "%s:%s",Desthost, splits[i]);
  }
  port=atoi(Destport);

	
	//Getaddrinfo
	struct addrinfo hints, *serverinfo = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if(getaddrinfo(Desthost, Destport, &hints, &serverinfo) < 0){
		printf("Getaddrinfo error: %s\n", strerror(errno)); 
		exit(0);
	} else printf("Getaddrinfo success\n");

  printf("Host %s, port %d\n",Desthost,port);
  
  printf("IPV: %d\n", serverinfo->ai_family);

  //Create socket
  sockfd = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
  if(sockfd == -1){
  	#ifdef DEBUG
  	printf("Socket Creation Failed. Exiting...\n");
  	#endif
  	exit(-1);
  }
  else{
  	#ifdef DEBUG
  	printf("Socket created successfully\n");
  	#endif
  }
		
	//Bind socket
  if((bind(sockfd, serverinfo->ai_addr, serverinfo->ai_addrlen)) < 0){
  	#ifdef DEBUG
  	printf("Socket bind failed. Exiting...\n");
  	#endif
  	exit(-1);
  }
  else {
  	#ifdef DEBUG
  	printf("Socket successfully bound\n");
  	#endif
  }
  /* 
     Prepare to setup a reoccurring event every 10s. If it_interval, or it_value is omitted, it will be a single alarm 10s after it has been set. 
  */
  struct itimerval alarmTime;
  alarmTime.it_interval.tv_sec=1;
  alarmTime.it_interval.tv_usec=1;
  alarmTime.it_value.tv_sec=1;
  alarmTime.it_value.tv_usec=1;

  /* Regiter a callback function, associated with the SIGALRM signal, which will be raised when the alarm goes of */
  signal(SIGALRM, checkJobbList);
  setitimer(ITIMER_REAL,&alarmTime,NULL); // Start/register the alarm. 

#ifdef DEBUG
  printf("DEBUGGER LINE ");
#endif
  struct calcProtocol clientProtocol;
  struct calcMessage compareMessage, clientMessage, errorMessage, acceptMessage;
  compareMessage.type = 22;
  compareMessage.message = 0;
  compareMessage.protocol = 17;
  compareMessage.major_version = 1;
  compareMessage.minor_version = 0;
  
  errorMessage.type = htons(2);
  errorMessage.message = htonl(2);
  errorMessage.protocol = htons(17);
  errorMessage.major_version = htons(1);
  errorMessage.minor_version = htons(0);
  
  acceptMessage.type = htons(2);
  acceptMessage.message = htonl(1);
  acceptMessage.protocol = htons(17);
  acceptMessage.major_version = htons(1);
  acceptMessage.minor_version = htons(0);
  int n;
  
  
  
  //The loop
  while(true)
  {
  
 	 //Peek MSG
 	 //printf("Listening\n");
 	 n = recvfrom(sockfd, (struct calcProtocol*)&clientProtocol, sizeof(calcProtocol), MSG_PEEK, (struct sockaddr*)&client, (socklen_t*)&len);
 	 if(n < 0){
  		#ifdef DEBUG
  		printf("Recvfrom failed: %s\n", strerror(errno));
  		#endif
 	 } 
 	 else{
  		#ifdef DEBUG
  		//printf("Bytes Recieved: %d\n", n);
  		#endif
  	}
  
  //If calcmsg add to list

 	 if(n <= 12){
   	 n = recvfrom(sockfd, (struct calcMessage*)&clientMessage, sizeof(calcMessage), 0, (struct sockaddr*)&client, (socklen_t*)&len);
  		if(n < 0){
  			#ifdef DEBUG
  			printf("Recvfrom failed: %s\n", strerror(errno));
  			#endif
  		} 
  		else{
  			#ifdef DEBUG
  			//printf("Bytes Recieved: %d\n", n);
  			#endif
  		}
  	
  		
  		//Check valid calcMessage
  		//Convert clientmessage from network byte order
  		clientMessage.type = ntohs(clientMessage.type);
 	 		clientMessage.message = ntohl(clientMessage.message);
  		clientMessage.protocol = ntohs(clientMessage.protocol);
  		clientMessage.major_version = ntohs(clientMessage.major_version);
  		clientMessage.minor_version = ntohs(clientMessage.minor_version);
  	
  		//Compare client message to reference
  		if(clientMessage.type == compareMessage.type && clientMessage.message == compareMessage.message && clientMessage.protocol == compareMessage.protocol && clientMessage.major_version == compareMessage.major_version && clientMessage.minor_version == compareMessage.minor_version){
  			#ifdef DEBUG
  			printf("ClientMessage Correct\n");
  			#endif
  		
  			//Add clientinfo
  			for(int i = 0;i<USERS;i++){
  				if(CLIENTS[i].PROTO.id == 0){
  					//Fill struct with client IP, Port, ID
						CLIENTS[i].CLIENTINFO = client;
						CLIENTS[i].PROTO.id = htons(nextID++);
						CLIENTS[i].LEN = len;
						CLIENTS[i].TIMER = 0;
					
						//Check if ID reached cap, if so then reset
						if(nextID >= 1000){
							nextID = 1;
						}
					
						#ifdef DEBUG
						printf("New Client!\n\tIP: %s\n\tPort %d\n\tID: %d\n", inet_ntoa(CLIENTS[i].CLIENTINFO.sin_addr), ntohs(CLIENTS[i].CLIENTINFO.sin_port), ntohs(CLIENTS[i].PROTO.id));
						#endif
					
					
						//Create the assignment and save output message
						createAssignment(CLIENTS[i].PROTO);
					
						//Send message
						if(sendto(sockfd, (struct calcProtocol*)&CLIENTS[i].PROTO, sizeof(calcProtocol), 0, (struct sockaddr*)&CLIENTS[i].CLIENTINFO, CLIENTS[i].LEN) < 0){
							printf("Error sending calcProtocol: %s\n", strerror(errno));
						}
						else printf("calcProtocol sent\n");
						break;
  			}
  		}
  		}
  	

  	
  	}
  	//If calcProto compare to list
  	else if(n > 12){
  		n = recvfrom(sockfd, (struct calcMessage*)&clientProtocol, sizeof(calcProtocol), 0, (struct sockaddr*)&client, (socklen_t*)&len);
  		if(n < 0){
  			#ifdef DEBUG
  			printf("Recvfrom failed: %s\n", strerror(errno));
  			#endif
  		} 
  		else{
  			#ifdef DEBUG
  			//printf("Bytes Recieved: %d\n", n);
  			#endif
  		}
  		
  		//Find client in list
  		char* searchIP = inet_ntoa(client.sin_addr);
  		int searchPORT = ntohs(client.sin_port);
  		int searchID = ntohs(clientProtocol.id);
  		bool foundClient = false;
  		for(int i = 0;i<USERS;i++){
  			if(strcmp(searchIP, inet_ntoa(CLIENTS[i].CLIENTINFO.sin_addr)) == 0 && searchPORT == ntohs(CLIENTS[i].CLIENTINFO.sin_port) && searchID == ntohs(CLIENTS[i].PROTO.id)){
  				#ifdef DEBUG
  				printf("Client has matching ID\n");
  				#endif
  				foundClient = true;
  				//Check if client calc is correct
  				
  				if(compareProtocols(CLIENTS[i].PROTO, clientProtocol)){
  					//Client has correctly calculated the message
  					//Send back OK msg and drop client
  					if(sendto(sockfd, (struct calcMessage*)&acceptMessage, sizeof(calcMessage), 0, (struct sockaddr*)&client, len) < 0){
							printf("Error sending accept msg: %s\n", strerror(errno));
						} else printf("Accept msg sent\n");
						
						
  				} 
  				else {
  					//Result not same, send error and drop client
  					//Send error, invalid client
  					if(sendto(sockfd, (struct calcMessage*)&errorMessage, sizeof(calcMessage), 0, (struct sockaddr*)&client, len) < 0){
						printf("Error sending error msg: %s\n", strerror(errno));
						} else printf("Error msg sent\n");
  				}
  				
  				//Drop client
  				memset(&CLIENTS[i], 0, sizeof(ClientInfo));
  				CLIENTS[i].TIMER = -1;
  				
  				break;
  			}
  		}
  		if(!foundClient){
  			//Send error, invalid client
  			if(sendto(sockfd, (struct calcMessage*)&errorMessage, sizeof(calcMessage), 0, (struct sockaddr*)&client, len) < 0){
					printf("Error sending error msg: %s\n", strerror(errno));
				} else printf("Error msg sent\n");
			
  		}
  		
  	}

  	
  	
  	n = 0;
  }
  return(0);


  
}
