//client co-ordinator

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>

#define BUFSIZE 2048 //constant size of message buffer
#define SERVICE_PORT 4400 //hard-coded port number where server resides
#define TCP_SERVICE_PORT 6600

/*Table that stores the IP, PortNo (of clients), Session_name */
struct session_tracker			
{
	char sess_name[9]; 	//Session name
	int tcp_port; 
};

/**********************************************
* Returns the length of the char array
***********************************************/
int getLength(char *ptr)
{
	char *ptr2;
	ptr2=ptr;
	int length=0;
	while(*ptr2!='\0')
	{
		length++;
		ptr2++;
	}
	return length;
}//end of getLength function 
/**********************************************
* Display all the active session names
***********************************************/
void displayActiveSessions(struct session_tracker s[], int j)
{
	int i=j;
	printf("\nThe active sessions you can join are :");
	for(i=0; i<50; i++)
	{
		if(strcmp(s[i].sess_name, "null")==0)
		{
			//do nothing
		}
		else
		{
			printf("\nSession %d : %s", i, s[i].sess_name);
		}
	}
} //end of displayActiveSessions function
		
	
int
main(int argc, char **argv)
{
	printf("*********Inside client-coordinator***********\n");
	struct sockaddr_in myaddr;					/* our address */
	struct sockaddr_in remaddr;					/* senders address -  to store */
	socklen_t addrlen = sizeof(remaddr);		/* length of addresses */
	unsigned char buf[BUFSIZE];					/* receive buffer */
	char s_name[9];								/*store the current session name in question */
	char *host = "localhost";					/* host to use if none supplied - change to use different server	*/
	int recvlen;								/* # bytes received */
	int UDPsock_cc;								/* our socket */
	int s_name_validity;						/* status of session name from client  [0 : already exists] [1: does not exist] */
	int i=0; 									/* to loop through the session table*/
	int flag_empty_row;							/*flag for an empty row*/
	
	/*Struct declaration*/
	struct session_tracker session_table[50];
	
	/* testing data */
	/*initialize all to null*/
	for(i=0; i<50; i++)
	{
		strcpy(session_table[i].sess_name, "null");
	}
	strcpy(session_table[0].sess_name, "abc");
	strcpy(session_table[20].sess_name, "what");
	strcpy(session_table[12].sess_name, "dance");
	
	/* create a UDP socket */
	//syntax : int file-descriptor = socket(int domain, int type, int protocol
	UDPsock_cc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (UDPsock_cc < 0)
	{
		perror("cannot create socket\n");
		return 0;
	}

	/*Before calling bind, this structure needs to be filled out.*/
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;	
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(SERVICE_PORT);
	
	/* bind the socket to any valid IP address and a specific port */
	if (bind(UDPsock_cc, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) 
	{
		perror("\nBind failed");
		return 0;
	}
	else
		printf("\nBind successful !");
	/*********************************************************************************************************************	
	/*Client_Cooordinator ready to accept messages 
	**********************************************************************************************************************/
	for(;;)
	{
		printf("\nThe session tracker contains : ");
		displayActiveSessions(session_table, 0);
		printf("\n\n**********Waiting on port %d ***************\n", SERVICE_PORT);
		//recieve the session name from the client
		recvlen = recvfrom(UDPsock_cc, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		if(recvlen > 0)
		{
			buf[recvlen] = 0;
			printf("received message: \"%s\" (%d bytes)\n", buf, recvlen);
			strcpy(s_name, buf);
			printf("\nbuf has been copied into s_name : %s", s_name);
			//check if the received message in "buf" is a valid session name or not.
			s_name_validity = 1; //default assumption that session does not exist in session_table
			for(i=0; i<50; i++)
			{
				if(recvlen==getLength(session_table[i].sess_name))
				{
					//only compare if the lengths are same
					if(strcmp(buf, session_table[i].sess_name)==0)
					{
						printf("\nThe session name already exists in the system.");
						s_name_validity = 0;
						break;
					}
				}
				//not the same hence
				//1. does not exist 
				//2. incorrect in case of Joining a session
				else 
				{
					s_name_validity = 1;
				}
			}
			printf("\nThe value of s_name_validity is %d", s_name_validity);
			printf("\nThe buffer value is : %s", buf);
		}
		else
		{
			printf("\nCould not recieve the s_name rom client");
		}
		/*wrap up the response s_name_validity and send to client*/
		sprintf(buf, "%d", s_name_validity);
		printf("\nThe buffer sent back to client contains : \"%s\"", buf);
		if((sendto(UDPsock_cc, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen))>0)
		{
			// recieve next step whether to start new session or join an existing session.	
			//buffer should contain s_name_create value : [0 : dont create] [1: create]
			if((recvlen = recvfrom(UDPsock_cc, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen))>=0)
			{	
				 printf("\nThe buffer on CC contains : %s ", buf);
				/***********************************************************************************************************/
				/*check the value of the buf (s_name_create)*/
				/***********************************************************************************************************/
				if(strcmp(buf, "1")==0)							/*new session will be created*/
				{
					printf("\nA new session will be forked !!");
					sprintf(buf, "%s", s_name);
					//add the s_name entry to the struct 
					for(i=0; i<50; i++)
					{
						//To check for the first null struct object and add the new s_name to it
						if(strcmp(session_table[i].sess_name, "null")==0)
						{
							flag_empty_row = i;
							break;
						}
					}
					strcpy(session_table[flag_empty_row].sess_name, s_name);
					printf("\nThe session_table[%d].sess_name is : %s", flag_empty_row, session_table[flag_empty_row].sess_name);
					printf("\n Session has been created !");
					sendto(UDPsock_cc, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
				}
				/***********************************************************************************************************/
				else if(strcmp(buf, "0")==0)					/*Session Match Found !*/
				{
					printf("\nJoining an existing session server");
					sprintf(buf, "%s", s_name);
					/* Add the client to the struct*/
					for(i=0; i<50; i++)
					{
						//To check for the first null struct object and add the new s_name to it
						if(strcmp(session_table[i].sess_name, "null")==0)
						{
							flag_empty_row = i;
							break;
						}
					}
					strcpy(session_table[flag_empty_row].sess_name, s_name);
					printf("\nThe session_table[%d].sess_name is : %s", flag_empty_row, session_table[flag_empty_row].sess_name);
				    printf("\n Session has been joined !");
					// send the newly added s_name to the client
					sendto(UDPsock_cc, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
				}
				else
				{
					printf("\ns_name_create does not have the correct value");
				}
			}
			else
			{
				printf("\nCould not recieve the correct s_name_create value!");
			}
		}
		else
		{	
			printf("\nCould not send s_name_validity to server ! ");
		}
		
	} //end of for loop
} //end of main function 
