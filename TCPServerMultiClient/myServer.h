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
typedef enum {initError,processError,closeError,success} serverErrors;
 serverErrors initServer ();
 serverErrors processServer();
 serverErrors closeServer();



#endif // MYSERVER_H_INCLUDED
