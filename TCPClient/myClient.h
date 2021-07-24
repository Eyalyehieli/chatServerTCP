#ifndef MYCLIENT_H_INCLUDED
#define MYCLIENT_H_INCLUDED
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8888
#define SA struct sockaddr
typedef enum {initError,processError,closeError,success} clientErrors;
 clientErrors initClient ();
 clientErrors processClient();
 clientErrors closeClient();


#endif // MYCLIENT_H_INCLUDED
