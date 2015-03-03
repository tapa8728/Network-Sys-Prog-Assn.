//Generate the session server TCP port number and returun it back to the client so client can connect its TCP port to it.
//Currently it is hardcoded

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif  /* INADDR_NONE */

#define	BUFLEN		128
#define PORT 4400
/****************************************************************************************************************** 
 *	This function is used to create UDP socket on the client side 
	parameters :
		host - address of CC
		port - Port of cc
	Goal : Create own UDP socket and connect to CC UDP socket
 ******************************************************************************************************************/
int createUDPSock(const char *host, const char *portnum)
{
	printf("Inside createUDPSock\n");
	int udpsock;
    /* Allocate a UDP socket */
	udpsock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpsock < 0)
	{
		printf("Socket cannot be created");
	}
       return udpsock;
  }
/****************************************************************************************************************** 
 *	This function is used to create TCP socket on the client side 
	parameters :
		host - address of CC
		port - Port of cc
	Goal : Create own UDP socket and connect to CC UDP socket
 ******************************************************************************************************************/
int createTCPSock(const char *host, const char *portnum)
{
	printf("\nInside createTCPSock");
	int tcpsock;
	tcpsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tcpsock < 0)
	{
		printf("Socket cannot be created");
	}
       return tcpsock;
}	
/*------------------------------------------------------------------------
 * main - create UDP socket to talk to client-coordinator
 *------------------------------------------------------------------------
 */
int main(int argc, char *argv[])
{
	struct sockaddr_in myaddr; 				/*UDP : address details about client*/
	struct sockaddr_in remaddr; 			/*UDP : address details of the CC*/
	struct sockaddr_in tcp_myaddr; 			/*TCP : address details about client*/
	struct sockaddr_in tcp_remaddr; 		/*TCP : address details of the CC*/
	char buf[BUFLEN];						/* message buffer for UDP*/
	char buffer[BUFLEN];					/* message buffer for TCP*/
	char s_name[10];						/* name of the session that client wants to connect to */
	char send_message[80];					/* Message to be sent by client to SS*/
	char *host = "127.0.0.1";				/* host to use if none supplied - change to use different server	*/
	char *portnum = "4400";					/* default client-coordinator port number	*/
	char *portnumtcp = "6600";					/* default client-coordinator port number	*/
	int fd, s_name_size, slen=sizeof(remaddr); 	/*fd - file descriptor*/
	int recvlen;							/* # bytes in acknowledgement message */
	int sentlen; 							/* # bytes in sendto message */
	int UDPsock, TCPsock, userInput;		/*userInput - user's input in the Main Menu */
	int userInputsess;						/*userInputsess - user's input in the Session Menu */
	int user_choice; 						/*if user wants to return to main menu or not */
	int sess_user_choice;					/*if user wants to return to session menu or not */
	int s_name_create; 						/*whether the CC should create new session or not [0 : dont create] [1: create] */
	
	printf("The cc host is : %s\n", host);
	printf("The cc portnum is : %s", portnum);
	
	/* calling the "createUDPSock" to create he UDP socket on the client side */
	UDPsock = createUDPSock(host, portnum);
	printf("\nReturned from createUDPSock : %d ",UDPsock);
	
	/*defining the address details for the UDP socket created on the client*/
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);  /*client port can be chosen by the system becuase it doesnt need to be wellknown*/
	if(bind(UDPsock, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
	{
		perror("bind failed");
		return 0;
	} 
	else
	{
		printf("\nBind successful on the client side :");
	}
	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */
	memset((char *)&remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	if ((remaddr.sin_port=htons((unsigned short)atoi(portnum))) == 0)
	{
        printf("\ncan't get \"%s\" port number\n", portnum);
	}
	else
	{
		printf("\nPort number was successfully set ");
	}
	
	/***********************************************************************************************************/
	/* MAIN MENU */
	/***********************************************************************************************************/
	do
	{
		printf("\n****************************************");
		printf("\n*******# WELCOME TO CHAT SYSTEM #*******");
		printf("\n****************************************");
		printf("\nSelect one of the following options -");
		printf("\n1. Start a new session");
		printf("\n2. Join an existing session");
		printf("\n3. Exit the chat system");
		printf("\nEnter a valid option from the menu : ");
		scanf("%d", &userInput);
		/*entering the userInput in a switch case */
		switch (userInput)
		{
			case 1:	//start new session
					printf("\n^^^^^^^^^^START NEW SESSION^^^^^^^^^^^");
					printf("\nEnter the session name : ");
					scanf("%s", s_name);
					printf("\nThe session_name is : %s", s_name);
					s_name_size = strlen(s_name);
					printf("\nThe s_name size is : %d", s_name_size);
					printf("\nSending packet %s to %s port %d\n", s_name, host, PORT);
					if (s_name_size <8) 	/*should be only 8 chars*/
					{
						/* adding the session name to the buffer */
						sprintf(buf, "%s", s_name);
						printf("\nThe buffer on client side contains s_name :%s", buf);
						/* sending the s_name to the cc */
						sentlen = sendto(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen);
						if(sentlen>0)
						{
							/* to receive s_name_validity value from server */
							recvlen = recvfrom(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, &slen);
							printf("The recvlen : %d", recvlen);
							if (recvlen>= 0)
							{
								buf[recvlen] = 0;	
								printf("\nReceived s_name_validity : \"%s\"\n", buf);
								/***********************************************************************************************************/
								if(strcmp(buf, "0")==0)					//session name already exists
								{
									s_name_create= 0;
									printf("\nSession name : \"%s\" already exists !! Please select \"Join a Session\" on MAIN MENU", s_name);
									//handled in case2: JOIN a new session
								}
								/***********************************************************************************************************/
								else if(strcmp(buf, "1")==0)			//session name does not exist and new one will be created
								{
									//send s_name_create =1 to CC to create new one
									s_name_create=1;
									sprintf(buf, "%d", s_name_create);
									printf("\nThe Buffer contains s_name_create: %s",buf);
									if((sendto(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)) >0)
									{
										/*Recieve "s_name" that has been newly created from CC*/
										if((recvlen = recvfrom(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, &slen)) >=0)
										{
											buf[recvlen] = 0;	/* expect a printable string - terminate it */
											printf("\nThe buffer contains : %s",buf);
											printf("\nYou are connected to the session %s \n", s_name);
										
										}//end of outer-if block
										else
										{
											printf("\nS_name could not be recieved successfully.");
										}
									}
									else
										printf("\ns_name_create could not be successfully sent");
								}  
								else
								{
									printf("\nCC has lost it !! ");
								}
							}
							else
							{
								printf("\n No message recieved ");
							}
						}
						else
						{
							printf("\nThe message could not be sent");
						}
					}
					else
					{
						printf("\nSession name should be upto 8 English Alphabets only\n");
					}
					break;
					
			case 2: //Join an existing session
					printf("\n^^^^^^^^^^JOIN A SESSION^^^^^^^^^^^");
					printf("\nEnter the session name : ");
					scanf("%s", s_name);
					printf("\nThe session_name is : %s", s_name);
					s_name_size = strlen(s_name);
					printf("\nThe s_name size is : %d", s_name_size);
					printf("\nSending packet %s to %s port %d\n", s_name, host, PORT);
					if(s_name_size <8) 	/*should be only 8 chars*/
					{
						/* adding the session name to the buffer */
						sprintf(buf, "%s", s_name);
						printf("\nThe buffer on client side contains :%s", buf);
						/* sending the s_name to the cc */
						sentlen = sendto(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen);
						printf("\nSentlen received from cc : %d", sentlen);
						if(sentlen>0)
						{
							/* to receive s_name_validity value from server */
							recvlen = recvfrom(UDPsock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, &slen);
							printf("The recvlen : %d", recvlen);
							if (recvlen>= 0)
							{
								buf[recvlen] = 0;	/* expect a printable string - terminate it */
								printf("received s_name_validity : \"%s\"\n", buf);
								if(strcmp(buf, "0")==0) 
								{
									//session name match found 
									s_name_create= 0;
									printf("\nSession name : \"%s\" has been found! Please wait while we connect you.....", s_name);
								}
								else if(strcmp(buf, "1")==0)
								{
									//User entered incorrect session name to join
									printf("\n The session name *%s* that you want to join does not exist !!" , s_name);
								}
								else
								{
									printf("\nCC has lost it !! ");
								}
							}
							else
							{
								printf("\ns_name_validity wasnt recieved.. ");
							}
						}
						else
						{
							printf("\nThe s_name could not be sent");
						}
					}
					else
					{
						printf("\nSession name should be upto 8 English Alphabets only\n");
					}
					break;
					
		    case 3: //exit chat system
					//call the exit function
					printf("\n^^^^^^^^^^EXIT CHAT SYSTEM^^^^^^^^^^^");
					printf("\nExiting System .. ");
					close(UDPsock);
					exit(0);
					break;
			default : printf("\nPlease enter a valid input");
		}//end of switch case
		printf("\nDo you want to continue with the chat-system? Enter 1 to continue : ");
		scanf("%d", &user_choice);
		printf("\nThe user chose:%d", user_choice);
	}while(user_choice == 1);	//end of do-while loop
}//end of main function


