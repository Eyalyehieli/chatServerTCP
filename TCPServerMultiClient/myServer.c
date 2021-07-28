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

    //serverErrors serverError=success;

	void addChildToSockets( myServerDataStruct* ds)
	{
	  int i=0;
	  for ( i = 0 ; i < ds->max_clients ; i++)
		{
			//socket descriptor
			ds->sd = ds->client_socket[i];

			//if valid socket descriptor then add to read list
			if(ds->sd > 0)
				FD_SET( ds->sd , &(ds->readfds));

			//highest file descriptor number, need it for the select function
			if(ds->sd > ds->max_sd)
				ds->max_sd = ds->sd;
		}
	}

	void processIncomingConnection( myServerDataStruct* ds)
	{
	int i=0;
	char *message = "ECHO Daemon v1.0 \r\n";
	if ((ds->new_socket = accept(ds->master_socket,(struct sockaddr *)&(ds->address), (socklen_t*)&(ds->addrlen)))<0)
			{
				perror("accept error");
				return processError;
			}

			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , ds->new_socket , inet_ntoa(ds->address.sin_addr) , ntohs
				(ds->address.sin_port));

			//send new connection greeting message
			if( send(ds->new_socket,message, strlen(message), 0) != strlen(message) )
			{
				perror("send error");
			}

			puts("Welcome message sent successfully");
			ds->valread = recv(ds->new_socket,ds->buffer,sizeof(ds->buffer),0);
			ds->buffer[ds->valread] = '\0';
            if(send(ds->new_socket , ds->buffer , strlen(ds->buffer) , 0 )!=strlen(ds->buffer))
            {
                perror("send error");

            }

			//add new socket to array of sockets
			for (i = 0; i < ds->max_clients; i++)
			{
				//if position is empty
				if( ds->client_socket[i] == 0 )
				{
					ds->client_socket[i] = ds->new_socket;
					printf("Adding to list of sockets as %d\n" , i);

					break;
				}
			}

	}

	void checkingIncomingMsg( myServerDataStruct* ds)
	{
	int i=0;
	for (i = 0; i < ds->max_clients; i++)
		{
			ds->sd = ds->client_socket[i];

			if (FD_ISSET( ds->sd , &(ds->readfds)))
			{
				//Check if it was for closing , and also read the
				//incoming message
				if ((ds->valread =recv(ds->sd,ds->buffer,sizeof(ds->buffer),0) == 0))
				{
					//Somebody disconnected , get his details and print
					getpeername(ds->sd , (struct sockaddr*)&(ds->address) , \
						(socklen_t*)&(ds->addrlen));
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(ds->address.sin_addr) , ntohs(ds->address.sin_port));

					//Close the socket and mark as 0 in list for reuse
					if(close(ds->sd)!=0)
					{
					  printf("close socket error");
					}
					ds->client_socket[i] = 0;
				}

				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					ds->buffer[ds->valread] = '\0';
					if(send(ds->sd , ds->buffer , strlen(ds->buffer) , 0 )!=strlen(ds->buffer))
					{
					    perror("send error");
					}
				}
			}
		}
	}


	serverErrors initServer( myServerDataStruct* ds)
	{
	int i=0;
	ds->opt=TRUE;
	ds->buffer=(char*)malloc(sizeof(BUFFSIZE));
	for (i = 0; i < ds->max_clients; i++)
	{
		ds->client_socket[i] = 0;
	}

	//cBUFFSIZEreate a master socket
	if( (ds->master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		return initError;
	}

	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(ds->master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&(ds->opt),
		sizeof(ds->opt)) < 0 )
	{
		perror("setsockopt");
		return initError;
	}

	//type of socket created
	ds->address.sin_family = AF_INET;
	ds->address.sin_addr.s_addr = INADDR_ANY;
	ds->address.sin_port = htons( PORT );

	//bind the socket to localhost port 8888
	if (bind(ds->master_socket, (struct sockaddr *)&(ds->address), sizeof(ds->address))<0)
	{
		perror("bind failed");
		return initError;
	}
	printf("Listener on port %d \n", PORT);

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(ds->master_socket, NUM_OF_PENDING_CLIENT ) < 0)
	{
		perror("listen");
		return initError;
	}

	//accept the incoming connection
	ds->addrlen = sizeof(ds->address);
	puts("Waiting for connections ...");
	return success;
	}

	serverErrors processServer( myServerDataStruct* ds)
	{
	   while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&(ds->readfds));

		//add master socket to set
		FD_SET(ds->master_socket, &(ds->readfds));
		ds->max_sd = ds->master_socket;

		addChildToSockets(ds);

		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		ds->activity = select( ds->max_sd + 1 , &(ds->readfds) , NULL , NULL , NULL);

		if ((ds->activity < 0) && (errno!=EINTR))
		{
			printf("select error");
			return processError;
		}

		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(ds->master_socket, &(ds->readfds)))
		{
			processIncomingConnection(ds);
		}

		//else its some IO operation on some other socket
		checkingIncomingMsg(ds);
	}
	return success;
	}

serverErrors closeServer( myServerDataStruct* ds)
{

  if(close(ds->master_socket)!=0)
  {
    return closeError;
  }
  return success;
}







