#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <calcLib.h>
#include <time.h>
#include "protocol.h"
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
//#define DEBUG

void calculate(struct calcProtocol &proto){

	int iValue1, iValue2, iResult, arith;
	float fResult, fValue1, fValue2;
	proto.type = htons(2);
	proto.major_version = htons(1);
	proto.minor_version = htons(0);
	arith = ntohl(proto.arith);
	iValue1 = ntohl(proto.inValue1); iValue2 = ntohl(proto.inValue2);
	fValue1 = proto.flValue1; fValue2 = proto.flValue2;
	if(arith == 1){
		printf("ASSIGNMENT: Add %d %d\n", iValue1, iValue2);
		iResult = iValue1 + iValue2;
		proto.inValue1 = htonl(iValue1);
		proto.inValue2 = htonl(iValue2);
		proto.inResult = htonl(iResult);
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 2){
		printf("ASSIGNMENT: Sub %d %d\n", iValue1, iValue2);
		iResult = iValue1 - iValue2;
		proto.inValue1 = htonl(iValue1);
		proto.inValue2 = htonl(iValue2);
		proto.inResult = htonl(iResult);
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 3){
		printf("ASSIGNMENT: Mul %d %d\n", iValue1, iValue2);
		iResult = iValue1 * iValue2;
		proto.inValue1 = htonl(iValue1);
		proto.inValue2 = htonl(iValue2);
		proto.inResult = htonl(iResult);
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 4){
		printf("ASSIGNMENT: Div %d %d\n", iValue1, iValue2);
		iResult = iValue1 / iValue2;
		proto.inValue1 = htonl(iValue1);
		proto.inValue2 = htonl(iValue2);
		proto.inResult = htonl(iResult);
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	
	else if (arith == 5){
		printf("ASSIGNMENT: fAdd %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 + fValue2;
		proto.flResult = fResult;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 6){
		printf("ASSIGNMENT: fSub %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 - fValue2;
		proto.flResult = fResult;
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 7){
		printf("ASSIGNMENT: fMul %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 * fValue2;
		proto.flResult = fResult;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 8){
		printf("ASSIGNMENT: fDiv %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 / fValue2;
		proto.flResult = fResult;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	

}

void restructure(struct calcProtocol &proto){
	//Convert everything then convert it back
	//Print for test?

	proto.type = htons(ntohs(proto.type));
	proto.major_version = htons(ntohs(proto.major_version));
	proto.minor_version = htons(ntohs(proto.minor_version));
	proto.id = htonl(ntohl(proto.id));
	proto.arith = htonl(ntohl(proto.arith));
	proto.inValue1 = htonl(ntohl(proto.inValue1));
	proto.inValue2 = htonl(ntohl(proto.inValue2));
	proto.inResult = htonl(ntohl(proto.inResult));

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
	int serverfd;
	struct sockaddr_in client;
	char server_message[CAP];
	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	
	
	
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
  printf("Host %s and port %d.\n",Desthost,port);
	
	//Getaddrinfo
	struct addrinfo hints, *serverinfo = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
	
	if(getaddrinfo(Desthost, Destport, &hints, &serverinfo) < 0){
		printf("Getaddrinfo error: %s\n", strerror(errno)); 
		exit(0);
	} //else printf("Getaddrinfo success\n");


  //Create UDP socket
  int socket_desc;
  struct sockaddr_in server_addr;
  socket_desc = socket(serverinfo->ai_family, serverinfo->ai_socktype, serverinfo->ai_protocol);
  socklen_t server_struct_length = sizeof(serverinfo);
  if(socket_desc < 0){
  	#ifdef DEBUG
  	printf("Unable to create socket\n");
  	#endif
  	return -1;
  }
  #ifdef DEBUG
  else printf("Socket Created\n");
  #endif
  
  if(setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
  	#ifdef DEBUG
  	printf("Socket option failed: %s\n", strerror(errno));
  	#endif
  }
 
  struct calcMessage calc_message;
  calc_message.type = htons(22);
  calc_message.message = htonl(0);
  calc_message.protocol = htons(17);
  calc_message.major_version = htons(1);
  calc_message.minor_version = htons(0);
  
  struct calcProtocol calc_protocol;
  
  //Create Socket Structure
  //server_addr.sin_family = AF_INET;
  //server_addr.sin_port = htons(port);
  //server_addr.sin_addr.s_addr = inet_addr(Desthost);
  int error;

 
  int result;
  

  
  int n = 0;
  int interval = 0;
  //Read server message 

	while(true){
	
  	//Send to
  	if(result = sendto(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0,
  			serverinfo->ai_addr, serverinfo->ai_addrlen) < 0){
  		#ifdef DEBUG
  		printf("Failed to send message\n");
  		#endif
  	}
  	else {
  		printf("Message sent\n");
  		interval++;
  	}
  	
  	n = recvfrom(socket_desc, &calc_protocol, sizeof(calc_protocol), 0, 
  		serverinfo->ai_addr, &serverinfo->ai_addrlen);
  	
  	if(n > 0){
  		break;
  	}
  	else if( n < 0){
  	#ifdef DEBUG
  		//printf("Failed to read msg. Errno: %s\n", strerror(errno));
  		#endif
			//exit(-1);
  
  	}
  	if(interval >= 3){
  		printf("Msg send failed 3 times. Exiting...\n");
  		exit(-1);
  	}
  	else{
  		if(ntohs(calc_protocol.type) == 2){
  		printf("Server sent NOT OK. Exiting...\n");
  		exit(-1);
  		}
  	}	
  	printf("Msg send failed %d times. Trying again...\n", interval);
	}
  
  //printf("Server: %s\n", server_message);
  
  char* answer = (char*)malloc(5);
  float calc;
  calculate(calc_protocol);


//sleep(11);
	interval = 0;
	n = 0;
	while(true){
	  //Send to
  	if(sendto(socket_desc, (struct calcProtocol*)&calc_protocol, sizeof(calcProtocol), 0,
  		serverinfo->ai_addr, serverinfo->ai_addrlen) < 0){
  		#ifdef DEBUG
  		printf("Failed to send message\n");
  		#endif
  		}
  	else{
  		//printf("Message sent\n");
  		interval++;
		}

  //Read server message 
  memset(&calc_message, 0, sizeof(calcMessage));
  n = recvfrom(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0, 
  	serverinfo->ai_addr, &serverinfo->ai_addrlen);
  if(n < 0){
  	#ifdef DEBUG
  		printf("Failed to read msg. Errno: %s\n", strerror(errno));
  		#endif
		 //exit(-1);
  }
  else if(n > 0){
  	break;
  }
  if(interval >= 3){
  		printf("Msg send failed 3 times. Exiting...\n");
  		exit(-1);
  	}
  
		
		printf("Msg send failed %d times. Trying again...\n", interval);
	}


  //else printf("Server Responded: %d\n", ntohl(calc_message.message));
  if(ntohl(calc_message.message) == 1){
  	printf("OK!\n");
  }
  else printf("ERROR\n");
}
