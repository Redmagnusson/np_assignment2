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

#define DEBUG

void calculate(struct calcProtocol &proto){

	int iValue1, iValue2, iResult, arith;
	float fResult, fValue1, fValue2;
	proto.type = htons(2);
	arith = ntohl(proto.arith);
	iValue1 = ntohl(proto.inValue1); iValue2 = ntohl(proto.inValue2);
	fValue1 = proto.flValue1; fValue2 = proto.flValue2;
	if(arith == 1){
		printf("ASSIGNMENT: Add %d %d\n", iValue1, iValue2);
		iResult = iValue1 + iValue2;
		proto.inResult = htonl(iResult);
		//char* answer = (char*)malloc(5);
		//sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 2){
		printf("ASSIGNMENT: Sub %d %d\n", iValue1, iValue2);
		iResult = iValue1 - iValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 3){
		printf("ASSIGNMENT: Mul %d %d\n", iValue1, iValue2);
		iResult = iValue1 * iValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	else if (arith == 4){
		printf("ASSIGNMENT: Div %d %d\n", iValue1, iValue2);
		iResult = iValue1 / iValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%d\n", iResult);
		//return answer;
	}
	
	else if (arith == 5){
		printf("ASSIGNMENT: fAdd %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 + fValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 6){
		printf("ASSIGNMENT: fSub %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 - fValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 7){
		printf("ASSIGNMENT: fMul %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 * fValue2;
		char* answer = (char*)malloc(5);
		sprintf(answer, "%8.8g\n", fResult);
		//return answer;
	}
	else if (arith == 4){
		printf("ASSIGNMENT: fDiv %8.8g %8.8g\n", fValue1, fValue2);
		fResult = fValue1 / fValue2;
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
int main(int argc, char *argv[]){
  
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  int port=atoi(Destport);
  char* server_message[2000];
  printf("Host %s, and Port %d.\n",Desthost,port);


  //Create UDP socket
  int socket_desc;
  struct sockaddr_in server_addr;
  socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
  
  
  //struct calcMessage{
  //	int type, message, protocol, major_version, minor_version;
  //};
  
  struct calcMessage calc_message;
  calc_message.type = htons(22);
  calc_message.message = htonl(0);
  calc_message.protocol = htons(17);
  calc_message.major_version = htons(1);
  calc_message.minor_version = htons(0);
  
  struct calcProtocol calc_protocol;
  
  //Create Socket Structure
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = inet_addr(Desthost);
  int error;

  
  //int msec = 0, trigger = 2000, iterations = 0;
  //clock_t before = clock();
  int result;
  
  //Send to
  if(result = sendto(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0,
  	(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
  		printf("Failed to send message\n");
  		}
  else printf("Message sent\n");
 
  //printf("Chars sent: %d \n", result);
  
  //Read server message 
  if(recvfrom(socket_desc, (struct calcProtocol*)&calc_protocol, sizeof(calcProtocol), 0, 
  	(struct sockaddr*)&server_addr, &server_struct_length) < 0){
  		printf("Failed to read msg. Errno: %s\n", strerror(errno));
		exit(-1);
  
  }
  else{
  	if(ntohs(calc_protocol.type) == 2){
  		printf("Server sent NOT OK. Exiting...\n");
  		exit(-1);
  	}
  }
  restructure(calc_protocol);
  
  char* answer = (char*)malloc(5);
  float calc;
  calculate(calc_protocol);
  
  
  //Send to
  if(result = sendto(socket_desc, (struct calcProtocol*)&calc_protocol, sizeof(calcProtocol), 0,
  	(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
  		printf("Failed to send message\n");
  		}
  else printf("Message sent\n");


  //Read server message 
  if(recvfrom(socket_desc, (struct calcProtocol*)&calc_message, sizeof(calcMessage), 0, 
  	(struct sockaddr*)&server_addr, &server_struct_length) < 0){
  		printf("Failed to read msg. Errno: %s\n", strerror(errno));
		exit(-1);
  
  }
  else printf("Server Responded: %d\n", ntohs(calc_message.type));
}
