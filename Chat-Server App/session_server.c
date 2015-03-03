//session server

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>

#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

#define	QLEN		  32
//#define SERVICE_PORT  6600 //hard-coded port number where server resides
#define BUFSIZE 2048 //constant size of message buffer

int main(int argc, char **argv)
{
		
		/***********************************************************************************************************/
		//step 3: listen for incoming requests
		if(listen(tcp_server, QLEN)<0)
			printf("\nStupid socket could not listen");
		else	
			printf("\n socket heard very well !! ");
		/***********************************************************************************************************/
		//step4: accept an incoming connection
		alen = sizeof(clientsin);
		printf("\n Session Server is ready and waiting !!");
		for(;;)
		{
			/************************************************************************************************************/
			//step5 : Accept a connection
			rmsg = accept(tcp_server, (struct sockaddr *)&clientsin, &alen);
			if(rmsg<0)
				printf("\nCannot acccept !!");
			else
				printf("\nSuccesfully accepted ! ");
			/***********************************************************************************************************/
			//step6 : Recieve a message - User's Choice !!
			if((recv(tcp_server, buf, strlen(buf), 0))<0)
				printf("\n Meessage - %s - was not recieved", buf);
			else
			{
				user_session_choice = atoi(buf);
				printf("\n user_session_choice :%d", user_session_choice);
				printf("\n MEssage - %s - was recieved successfully!! YAY", buf);
				switch(user_session_choice)
				{
					/**********************************************************************************************************
					case 1: //client wants to submit a message
							printf("\n SS Inside Submit message to \"%s\" ", buf);
							sprintf(buf, "%d", 0); //<---- successful connection 
							//step7: Send "0" to client for successful connection
							if((send(tcp_server, buf, strlen(buf), 0))<0)
								printf("\n Message - %s - was not sent to client", buf);
							else
								printf("\n Message - %s - was sent to client!! YAY", buf);
							//step8: Recieve message to be submmitted to session in struct
							if((recv(tcp_server, buf, strlen(buf), 0))<0)
								printf("\n MEssage - %s - was not recieved", buf);
							else
							{
								strcpy(send_message, buf);
								printf("\n Message was recieved(submit message): %s", buf);
								printf("\n SS will add (send_message, s_name) to the struct");
							}
							break;							
					/***********************************************************************************************************
					case 2: //client wants to get next message
							printf("\n SS Inside Get Next message in \"%s\" ", buf);
							break;
					/***********************************************************************************************************
					case 3: //client wants to get all messages
							printf("\n Get All messages in \"%s\" ", buf);
							break;		
					/***********************************************************************************************************
					case 4: //client wants to leave session
							printf("\n SS Leave session \"%s\" ", buf);
							break;
					/***********************************************************************************************************
					default: //incorrect input
							printf("\n SS incorrect option! Try again !!");
							break;
				}// end of switch */
			}	
			
		}
}
