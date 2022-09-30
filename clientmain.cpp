#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <calcLib.h>
#include <time.h>
#include "protocol.h"

#define DEBUG

char* calculate(struct calcProtocol &proto){

	int iValue1, iValue2, iResult, arith;
	float fResult, fValue1, fValue2;
	char* returnChar;
	
	arith = ntohl(proto.arith);
	iValue1 = ntohl(proto.inValue1); iValue2 = ntohl(proto.inValue2);
	fValue1 = proto.flValue1; fValue2 = proto.flValue2;
	if(arith == 1){
		printf("ASSIGNMENT: Add %d %d\n", iValue1, iValue2);
		iResult = iValue1 + iValue2;
		sprintf(returnChar, "%d\n", iResult);
	}
	else if (arith == 2){
		printf("ASSIGNMENT: Sub %d %d\n", iValue1, iValue2);
		iResult = iValue1 - iValue2;
		sprintf(returnChar, "%d\n", iResult);
	}
	else if (arith == 3){
		printf("ASSIGNMENT: Mul %d %d\n", iValue1, iValue2);
		iResult = iValue1 * iValue2;
		sprintf(returnChar, "%d\n", iResult);
	}
	else if (arith == 4){
		printf("ASSIGNMENT: Div %d %d\n", iValue1, iValue2);
		iResult = iValue1 / iValue2;
		sprintf(returnChar, "%d\n", iResult);
	}
	
	else if (arith == 5){
		printf("ASSIGNMENT: fAdd %8.8g\n", fValue1, fValue2);
		fResult = fValue1 + fValue2;
		sprintf(returnChar, "%8.8g\n", fResult);
	}
	else if (arith == 6){
		printf("ASSIGNMENT: fSub %8.8g\n", fValue1, fValue2);
		fResult = fValue1 - fValue2;
		sprintf(returnChar, "%8.8g\n", fResult);
	}
	else if (arith == 7){
		printf("ASSIGNMENT: fMul %8.8g\n", fValue1, fValue2);
		fResult = fValue1 * fValue2;
		sprintf(returnChar, "%8.8g\n", fResult);
	}
	else if (arith == 4){
		printf("ASSIGNMENT: fDiv %8.8g\n", fValue1, fValue2);
		fResult = fValue1 / fValue2;
		sprintf(returnChar, "%8.8g\n", fResult);
	}
	
	return returnChar;

}


int main(int argc, char *argv[]){
  
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  struct sockaddr_in servaddr;
  char server_message[2000], client_message[2000];
  int port=atoi(Destport);
  

  printf("Host %s, and Port %d.\n",Desthost,port);


  //Create UDP socket
  int socket_desc;
  struct sockaddr_in server_addr;
  socket_desc = socket(AF_INET, SOCK_DGRAM, 0);
  //int server_struct_length = sizeof(server_addr);
  socklen_t server_struct_length = sizeof(server_addr);
  if(socket_desc < 0){
  	#ifdef DEBUG
  	printf("Unable to create socket\n");
  	#endif
  	return -1;
  }
  #ifdef DEBUG
  else printf("Socket Created\n");
  #endif
  
  
  struct calcMessage{
  	int type, message, protocol, major_version, minor_version;
  };
  
  struct calcMessage calc_message;
  calc_message.type = htons(22);
  calc_message.message = htons(0);
  calc_message.protocol = htons(17);
  calc_message.major_version = htons(1);
  calc_message.minor_version = htons(0);
  
  struct calcProtocol calc_protocol;
  
  //Create Socket Structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(Desthost);
  int error;

  
  int msec = 0, trigger = 2000, iterations = 0;
  clock_t before = clock();
  int result;
  
  do{
  
  	//Send to
  	if(result = sendto(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0,
  		(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
  			printf("Failed to send message\n");
  		}
  	else printf("Message sent\n");
  	
  clock_t difference = clock() - before;
  msec = difference * 1000 / CLOCKS_PER_SEC;
  iterations++;  
  if(iterations > 2 || result > 0) break;
  }while(msec < trigger);
  
  //Read server message 
  if(recvfrom(socket_desc, (struct calcProtocol*)&calc_protocol, sizeof(calc_protocol), 0, 
  	(struct sockaddr*)&server_addr, &server_struct_length) < 0){
		exit(-1);
  
  }
  else{
  	if(ntohs(calc_protocol.type) == 2){
  		printf("Server sent NOT OK. Exiting...\n");
  		exit(-1);
  	}
  }
  
  char* ans = calculate(calc_protocol);
  
  msec = 0; iterations = 0; result = 0;
    do{
  
  	//Send to
  	if(result = sendto(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0,
  		(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
  			printf("Failed to send message\n");
  		}
  	else printf("Message sent\n");
  	
  clock_t difference = clock() - before;
  msec = difference * 1000 / CLOCKS_PER_SEC;
  iterations++;  
  if(iterations > 2 || result > 0) break;
  }while(msec < trigger);

}
