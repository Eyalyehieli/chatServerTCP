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
#define BUFFSIZE 1025
#define MAXCLIENTS 30
#define NUM_OF_PENDING_CLIENT 3

    serverErrors serverError=success;
    static int opt = TRUE;
	static int master_socket , addrlen , new_socket , client_socket[MAXCLIENTS] ,
		max_clients = MAXCLIENTS , activity, valread , sd;
	static int max_sd;
	static struct sockaddr_in address;

	static char* buffer; //data buffer of 1K

	//set of socket descriptors
	static fd_set readfds;

	//a message		}

	static char *message = "ECHO Daemon v1.0 \r\n";

	void addChildToSockets()
	{
	  int i=0;
	 for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i];

			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);

			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	}

	void processIncomingConnection()
	{
	int i=0;
	if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept error");
				return processError;
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
				(address.sin_port));

			//send new connection greeting message
			if( send(new_socket, message, strlen(message), 0) != strlen(message) )
			{
				perror("send error");

			}

			puts("Welcome message sent successfully");
			valread = recv(new_socket,buffer,sizeof(buffer),0);
			buffer[valread] = '\0';
            if(send(new_socket , buffer , strlen(buffer) , 0 )!=strlen(buffer))
            {
                perror("send error");

            }

			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i] == 0 )
				{
					client_socket[i] = new_socket;
					printf("Adding to list of sockets as %d\n" , i);

					break;
				}
			}

	}

	void checkingIncomingMsg()
	{
	int i=0;
	for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i];

			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((valread =recv(sd,buffer,sizeof(buffer),0) == 0))
				{
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					if(close(sd)!=0)
					{
					  printf("close socket error");
					}
					client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					buffer[valread] = '\0';
					if(send(sd , buffer , strlen(buffer) , 0 )!=strlen(buffer))
					{
					    perror("send error");
					}
				}
			}
		}
	}


	serverErrors initServer()
	{
	int i=0;
	buffer=(char*)malloc(sizeof(BUFFSIZE));
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i] = 0;
	}

	//cBUFFSIZEreate a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		return initError;
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		return initError;
	}

	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		return initError;
	}
	printf("Listener on port %d \n", PORT);

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, NUM_OF_PENDING_CLIENT ) < 0)
	{
		perror("listen");
		return initError;
	}

	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	return success;
	}

	serverErrors processServer()
	{
	   while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);

		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;

		addChildToSockets();

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
			return processError;
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{
			processIncomingConnection();
		}

		//else its some IO operation on some other socket
		checkingIncomingMsg();
	}
	return success;
	}

serverErrors closeServer()
{

  if(close(master_socket)!=0)
  {
    return closeError;
  }
  return success;
}







