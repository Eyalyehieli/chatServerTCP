#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8888
#define SA struct sockaddr
#include "myClient.h"


int main()
{
    clientErrors check;
	check=initClient();
	if(check==success)
	{
	 check=processClient();
	 if(check==processError)
	 {
	 printf("process client error");
	 }
    }
	else
	{
	  printf("init client error");
	}
	check=closeClient();
	if(check==closeError)
	{
	  printf("close client error");
	}
}
