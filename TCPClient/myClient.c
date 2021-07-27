#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 8888
#define SA struct sockaddr
#include "myClient.h"
 int sockfd;
 clientErrors clientError=success;

clientErrors initClient()
{

int connfd;
	struct sockaddr_in servaddr, cli;
	char buff[1025];

	// socket create and varification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		clientError=initError;
		return clientError;
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		clientError=initError;
		return clientError;
	}
	else
		printf("connected to the server..\n");


	// function for chat
	recv(sockfd,buff,sizeof(buff),0);
	//read(sockfd, buff, sizeof(buff));
    printf("%s",buff);
    return success;
}

 clientErrors processClient()
{
	char buff[MAX];
	int n;
	for (;;) {
		bzero(buff, sizeof(buff));
		n = 0;
		printf("Enter the string : ");
		while ((buff[n++] = getchar()) != '\n');
		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		recv(sockfd, buff, sizeof(buff),0);
		//read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}
	return clientError;
}

 clientErrors closeClient()
{

  if(close(sockfd)!=0)
  {
  clientError=closeError;
  }
  return clientError;
}
