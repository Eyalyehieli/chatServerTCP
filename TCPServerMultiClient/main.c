//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
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
#include "myServer.h"

#define TRUE 1
#define FALSE 0
#define PORT 8888

int main(int argc , char *argv[])
{
    serverErrors check;
    myServerDataStruct* ds=(myServerDataStruct*)malloc(sizeof(myServerDataStruct));
	check=initServer(ds);
	if(check==success)
	{
	 check=processServer(ds);
	 if(check==processError)
	 {
	 printf("process server error");
	 }
    }
	else
	{
	  printf("init server error");
	}
	check=closeServer(ds);
	if(check==closeError)
	{
	  printf("close server error");
	}

	return 0;
}
