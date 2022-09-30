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
  int server_struct_length = sizeof(server_addr);
  
  if(socket_desc < 0){
  	#ifdef DEBUG
  	printf("Unable to create socket\n");
  	#endif
  	return -1;
  }
  #ifdef DEBUG
  else printf("Socket Created\n");
  #endif
  
  
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
  
  //Read server message TODO this doesnt work, last argument is invalid
  if(recvfrom(socket_desc, server_message, sizeof(server_message), 0, 
  	(struct sockaddr*)&server_addr, (socklen_t*)sizeof(server_addr)) < 0){
  		//Do stuff
  
  } else printf(server_message);
  
  
  //Send to
  if(sendto(socket_desc, client_message, strlen(client_message), 0,
  	(struct sockaddr*)&server_addr, server_struct_length) < 0){
  		//Do stuff
  	}
}
