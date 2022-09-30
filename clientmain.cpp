#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <calcLib.h>
#include "protocol.h"

#define DEBUG

int main(int argc, char *argv[]){
  
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  struct sockaddr_in servaddr;
  char server_message[2000], client_message[2000];
  int port=atoi(Destport);
  
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
  
  //Establish Connection
  error = connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(error < 0){
  	#ifdef DEBUG
  	printf("Unable to connect\n");
  	//printf("Error: %d \n", errno);
  	#endif
  	return -1;
  }
  #ifdef DEBUG
  else printf("Connected\n");
  #endif
  
  //Send to
  if(sendto(socket_desc, (struct calcMessage*)&calc_message, sizeof(calcMessage), 0,
  	(struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
  		printf("Failed to send message\n");
  	}
  else printf("Message sent\n");
  
  //Read server message TODO this doesnt work, last argument is invalid
  if(recvfrom(socket_desc, (struct calcProtocol*)&calc_protocol, sizeof(calc_protocol), 0, 
  	(struct sockaddr*)&server_addr, &server_struct_length) < 0){
  		//Do stuff
		printf("Didn't work\n");
  
  }
  else{
  	printf("Type: %d \n", ntohs(calc_protocol.type));
  	printf("Major_version: %d \n", ntohs(calc_protocol.major_version));
  	printf("Minor_version: %d \n", ntohs(calc_protocol.minor_version));
  	printf("ID: %u\n", ntohl(calc_protocol.id));
  	printf("Arith: %u\n", ntohl(calc_protocol.arith));
  	printf("Int1: %u\n", ntohl(calc_protocol.inValue1));
  	printf("Int2: %u\n", ntohl(calc_protocol.inValue2));
  	printf("Float1: %f\n", calc_protocol.flValue1);
  	printf("Float2: %f\n", calc_protocol.flValue2);
  	printf("Result: %f\n", calc_protocol.flResult);
  }
  
  

}
