#ifndef MYSERVER_H_INCLUDED
#define MYSERVER_H_INCLUDED
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros

#define TRUE 1
#define FALSE 0
#define PORT 8888
#define MAXCLIENTS 30

typedef enum {initError,processError,closeError,success} serverErrors;
typedef struct{ int opt;
	             int master_socket;
	             int addrlen;
	             int new_socket;
	             int client_socket[MAXCLIENTS];
		         int max_clients;
		         int activity;
                 int valread;
                 int sd;
                 int max_sd;
	             struct sockaddr_in address;
                 char* buffer; //data buffer of 1K
	//set of socket descriptors
                 fd_set readfds;
 }myServerDataStruct;
 serverErrors initServer ( myServerDataStruct*);
 serverErrors processServer( myServerDataStruct*);
 serverErrors closeServer( myServerDataStruct*);
 void addChildToSockets( myServerDataStruct*);
 void processIncomingConnection( myServerDataStruct*);
 void checkingIncomingMsg( myServerDataStruct*);




#endif // MYSERVER_H_INCLUDED
