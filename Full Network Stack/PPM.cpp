#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include<iostream>
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>

using namespace std;

#include "message.h"
#include "threadpool.h"

#define BUFSIZE 2048

/********************************STRUCTURES FOR HEADERS***********************************/

struct EthernetHeader	//1
 {
	int hlp;	
	char oi[8];
	int msglen; 
 };

struct IPHeader	//2
 {
	int hlp;	
	char oi[12];
	int msglen; 
 };

struct TCPHeader	//3
 {
	int hlp;
	char oi[4];
	int msglen;
 };

struct UDPHeader	//4
 {
	int hlp;	
	char oi[4];
	int msglen; 
 };

struct FTPHeader	//5
 {
	int hlp;	
	char oi[8];
	int msglen; 
 };

struct TelnetHeader	//6
 {
	int hlp;	
	char oi[8];
	int msglen; 
 };

struct RDPHeader	//7
 {
	int hlp;	
	char oi[12];
	int msglen; 
 };

struct DNSHeader	//8
 {
	int hlp;	
	char oi[8];
	int msglen; 
 };

struct unit
 {
	int proto_id;
	Message *mp;
 };

/*******************************SEND() AND DELIVER() FUNCTIONS*************************************/
void send_FTP(int arg1, Message *arg2);
void send_Telnet(int arg1, Message *arg2);
void send_RDP(int arg1, Message *arg2);
void send_DNS(int arg1, Message *arg2);
void send_TCP(int arg1, Message *arg2);
void send_UDP(int arg1, Message *arg2);
void send_IP(int arg1, Message *arg2);
void send_Ethernet(int arg1, Message *arg2);

void deliver_Ethernet(void *arg);
void deliver_IP(Message *arg);
void deliver_TCP(Message *arg);
void deliver_UDP(Message *arg);
void deliver_FTP(Message *arg);
void deliver_Telnet(Message *arg);
void deliver_RDP(Message *arg);
void deliver_DNS(Message *arg);
int send_FTP_counter=0, send_Telnet_counter=0, send_RDP_counter=0, send_DNS_counter=0;
int deliver_FTP_counter=0, deliver_Telnet_counter=0, deliver_RDP_counter=0, deliver_DNS_counter=0;

pthread_t IN_sock_RecvThread;
pthread_t FTPapp_RecvThread, Telnetapp_RecvThread, RDPapp_RecvThread, DNSapp_RecvThread;

struct sockaddr_in in_addr;	
struct sockaddr_in out_addr;
struct sockaddr_in remaddr;
socklen_t out_addrlen = sizeof(out_addr);	
socklen_t in_addrlen = sizeof(in_addr);
int UDP_IN_sock, UDP_OUT_sock;	
int UDP_IN_port;	
int UDP_OUT_port; 
int port1, port2;
int ret;
int rec;

char buf[BUFSIZE];	

void *send_array[8];
void *deliver_array[8];

ThreadPool th(25);

class network
 {

	public:
	network();	
 };

network:: network()
 {
	//cout<<"this constructor does nothing"<<endl;	
 }


/******************************Application corresponding to FTP*************************************/

void *FTPapp(void *arg)
 {
	for(int i=0;i<10;i++)
	 {
		cout<<"Inside the FTP application "<<endl;
		char *b1 = new char[100];
    	for (int i = 0; i < 100; i++) 
				b1[i] = 'f';
		Message *m = new Message(b1, 100);
		char test1[140];
		m->msgFlat(test1);
		test1[m->msgLen ( )] = '\n';
		cout << "Message written to FTP by the application is: " << test1 << endl;
		send_FTP(5, m);
		usleep(100);
	 }
	
 }

/******************************Application corresponding to Telnet*************************************/

void *Telnetapp(void *arg)
 {
	for(int i=0;i<10;i++)
	 {
		cout<<"Inside the Telnet application "<<endl;
		char *b1 = new char[100];
    	for (int i = 0; i < 100; i++) 
				b1[i] = 't';
		Message *m = new Message(b1, 100);
		char test1[140];
		m->msgFlat(test1);
		test1[m->msgLen ( )] = '\n';
		cout << "Message written to Telnet by the application is: " << test1 << endl;
		send_Telnet(6, m);
		usleep(100);
	 }
 }

/******************************Application corresponding to RDP*************************************/

void *RDPapp(void *arg)
 {
	for(int i=0;i<10;i++)			
	 {	 
		cout<<"Inside the RDP application "<<endl;
		char *b1 = new char[100];
    	for (int i = 0; i < 100; i++) 
				b1[i] = 'r';
		Message *m = new Message(b1, 100);
		char test1[140];
		m->msgFlat(test1);
		test1[m->msgLen ( )] = '\n';
		cout << "Message written to RDP by the application is: " << test1 << endl;
		send_RDP(7, m);
		usleep(100);
	 }
 }

/******************************Application corresponding to DNS Client*************************************/

void *DNSapp(void *arg)
 {
	for(int i=0;i<10;i++)
	 {
		cout<<"Inside the DNS Client application"<<endl;
		char *b1 = new char[100];
    	for (int i = 0; i < 100; i++) 
				b1[i] = 'd';
		Message *m = new Message(b1, 100);
		char test1[140];
		m->msgFlat(test1);
		test1[m->msgLen ( )] = '\n';
		cout << "Message written to DNS Client by the application is: " << test1 << endl;
		send_DNS(8, m);
		usleep(100);
	 }
 }

/****************************************************************************************************************
*******************************************SEND() FUNCTIONS******************************************************
*****************************************************************************************************************/

void send_FTP(int arg1, Message *arg2)
 {
	//build header	
	cout<<"Inside send_FTP"<<endl;
	FTPHeader *ftph = new FTPHeader;
	ftph->hlp=arg1;
	snprintf(ftph->oi, 8, "ETHERNEsassassssss");
	ftph->msglen=arg2->msgLen();
	cout<<"Length of existing message is in FTP is "<<ftph->msglen<<endl;
	//convert header to char*
	//add the header
	arg2->msgAddHdr((char *)ftph, sizeof(FTPHeader));	
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	//send to next protocol
	send_TCP(5, arg2);
 }

void send_Telnet(int arg1, Message *arg2)
 {	
	//build Telnet header
	cout<<"Inside send_Telnet"<<endl;
	TelnetHeader *telneth = new TelnetHeader;
	telneth->hlp=arg1;
	snprintf(telneth->oi, 8, "ETHERNEsassassssss");
	telneth->msglen=arg2->msgLen();
	cout<<"Length of existing message in Telnet is "<<telneth->msglen<<endl;
	arg2->msgAddHdr((char *)telneth, sizeof(TelnetHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_TCP(6, arg2);
 }

void send_RDP(int arg1, Message *arg2)
 {
	//build RDP header
	cout<<"Inside send_RDP"<<endl;
	RDPHeader *rdph = new RDPHeader;
	rdph->hlp=arg1;
	snprintf(rdph->oi, 12, "ETHERNEsassassssss");
	rdph->msglen=arg2->msgLen();
	cout<<"Length of existing message in RDP is "<<rdph->msglen<<endl;
	arg2->msgAddHdr((char *)rdph, sizeof(RDPHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_UDP(7, arg2);
 }

void send_DNS(int arg1, Message *arg2)
 {
	//build DNS header
	cout<<"Inside send_DNS"<<endl;
	DNSHeader *dnsh = new DNSHeader;
	dnsh->hlp=arg1;
	snprintf(dnsh->oi, 8, "ETHERNEsassassssss");
	dnsh->msglen=arg2->msgLen();
	cout<<"Length of existing message in DNS is "<<dnsh->msglen<<endl;
	arg2->msgAddHdr((char *)dnsh, sizeof(DNSHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_UDP(8, arg2);
 }

void send_TCP(int arg1, Message *arg2)
 {
	//build TCP header
	cout<<"Inside send_TCP"<<endl;
	TCPHeader *tcph = new TCPHeader;
	tcph->hlp=arg1;
	snprintf(tcph->oi, 4, "ETHERNEsassassssss");
	tcph->msglen=arg2->msgLen();
	cout<<"Length of existing message in TCP is "<<tcph->msglen<<endl;
	arg2->msgAddHdr((char *)tcph, sizeof(TCPHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_IP(3, arg2);
 }


void send_UDP(int arg1, Message *arg2)
 {
	//build UDP header
	cout<<"Inside send_UDP"<<endl;
	UDPHeader *udph = new UDPHeader;
	udph->hlp=arg1;
	snprintf(udph->oi, 4, "ETHERNEsassassssss");
	udph->msglen=arg2->msgLen();
	cout<<"Length of existing message in UDP is "<<udph->msglen<<endl;
	arg2->msgAddHdr((char *)udph, sizeof(UDPHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_IP(4, arg2);
 }


void send_IP(int arg1, Message *arg2)
 {
	//build IP header
	cout<<"Inside send_IP"<<endl;
	IPHeader *iph = new IPHeader;
	iph->hlp=arg1;
	snprintf(iph->oi, 12, "ETHERNEsassassssss");
	iph->msglen=arg2->msgLen();
	cout<<"Length of existing message in IP is "<<iph->msglen<<endl;
	arg2->msgAddHdr((char *)iph, sizeof(IPHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	send_Ethernet(2, arg2);
 }

void send_Ethernet(int arg1, Message *arg2)
 {
	//build Ethernet header
	cout<<"Inside send_Ethernet"<<endl;
	EthernetHeader *ethh = new EthernetHeader;
	ethh->hlp=arg1;
	snprintf(ethh->oi, 8, "ETHERNEsassassssss");
	ethh->msglen=arg2->msgLen();
	cout<<"Length of existing message in Ethernet is "<<ethh->msglen<<endl;
	arg2->msgAddHdr((char *)ethh, sizeof(EthernetHeader));
	cout << "Message length of arg2 = " << arg2->msgLen( ) << endl;
	char buf1[200];
    arg2->msgFlat(buf1);
	buf1[arg2->msgLen()] = '\n';	
	if (sendto(UDP_OUT_sock, buf1, sizeof(buf1), 0, (struct sockaddr *)&remaddr, sizeof(remaddr))<0)
	 { 
		perror("sendto");
		exit(1);
	 }
	cout<<"Message sent to UDP_OUT_sock"<<endl;
	send_FTP_counter++;
	cout<<"The number of messages sent are: "<<send_FTP_counter<< endl;
	
 } 

/*************************************IN_sock_Receive Function**************************************************/
void *IN_sock_RecvFunc(void *arg)
 {
	socklen_t remlen = sizeof(remaddr);
	cout<<"Inside the IN_sock_recv function"<<endl;
	while(1)
	{
		if(rec=recvfrom(UDP_IN_sock, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &remlen)>0)
	 	{
			buf[rec]='\0';
			Message *m= new Message(buf, sizeof(buf));
			cout<<"Message len is :"<< m->msgLen() <<endl;
			if(th.thread_avail())
				th.dispatch_thread(deliver_Ethernet, (void *)m);
			else
				cout << "No thread is available for the function"<< endl; 
			//deliver_Ethernet((Message *)m);
			sleep(1);
			cout<<"End of recvfrom.. lets go back up!"<<endl;
		 }
	 }
}

/****************************************************************************************************************
********************************************DELIVER() FUNCTIONS**************************************************
*****************************************************************************************************************/

void deliver_Ethernet(void *arg)
 {
	cout<<"Inside deliver_Ethernet "<<endl;
	Message *eth = (Message *)arg;
	char *Eth_strip = new char[sizeof(EthernetHeader)];
	Eth_strip = (eth)->msgStripHdr(sizeof(EthernetHeader));
	EthernetHeader *Eth_stripped = new EthernetHeader;
	Eth_stripped =(EthernetHeader *)Eth_strip;
	cout<<"Ethernet Stripped HLP is-  "<<Eth_stripped->hlp<<endl;
	deliver_IP(eth);
 }

void deliver_IP(Message *arg)
 {
	cout<<"Inside deliver_IP "<<endl;	
	Message *ip = (Message *)arg;
	int ID;
	char *IP_strip = new char[sizeof(IPHeader)];
	IP_strip = (ip)->msgStripHdr(sizeof(IPHeader));
	IPHeader *IP_stripped = new IPHeader;
	IP_stripped =(IPHeader *)IP_strip;
	ID= IP_stripped->hlp;
	cout<<"IP Stripped HLP is-  "<<IP_stripped->hlp<<endl;
	if(ID==3)
 		deliver_TCP(ip);
	else if(ID==4)
		deliver_UDP(ip);
 }

void deliver_TCP(Message *arg)
 {
	cout<<"Inside deliver_TCP "<<endl;	
	int ID;
	Message *tcp = (Message *)arg;
	char *TCP_strip = new char[sizeof(TCPHeader)];
	TCP_strip = (tcp)->msgStripHdr(sizeof(TCPHeader));
	TCPHeader *TCP_stripped = new TCPHeader;
	TCP_stripped =(TCPHeader *)TCP_strip;
	ID=TCP_stripped->hlp;
	cout<<"TCP Stripped HLP is-  "<<TCP_stripped->hlp<<endl;
	if(ID==5)
 		deliver_FTP(tcp);
	else if(ID==6)
		deliver_Telnet(tcp);
 }

void deliver_UDP(Message *arg)
 {
	cout<<"Inside deliver_UDP "<<endl;	
	int ID;
	Message *udp = (Message *)arg;
	char *UDP_strip = new char[sizeof(UDPHeader)];
	UDP_strip = (udp)->msgStripHdr(sizeof(UDPHeader));
	UDPHeader *UDP_stripped = new UDPHeader;
	UDP_stripped =(UDPHeader *)UDP_strip;
	ID=UDP_stripped->hlp;
	cout<<"UDP Stripped HLP is-  "<<UDP_stripped->hlp<<endl;
	if(ID==7)
 		deliver_RDP(udp);
	else if(ID==8)
		deliver_DNS(udp);
 }

void deliver_FTP(Message *arg)
 {
	cout<<"Inside deliver_FTP "<<endl;	
	Message *ftp = (Message *)arg;
	char *FTP_strip = new char[sizeof(FTPHeader)];
	FTP_strip = (ftp)->msgStripHdr(sizeof(FTPHeader));
	FTPHeader *FTP_stripped = new FTPHeader;
	FTP_stripped =(FTPHeader *)FTP_strip;
	char bufy[140];
	bzero(bufy,140);
	(ftp)->msgFlat(bufy);
    bufy[(ftp)->msgLen ( )] = '\n';
    cout << "Message Received by FTP is: " << bufy << endl;	
	deliver_FTP_counter++;
	cout<<"FTP received " << deliver_FTP_counter << " messages"<< endl;
	IN_sock_RecvFunc(arg);
 }

void deliver_Telnet(Message *arg)
 {
	cout<<"Inside deliver_Telnet "<<endl;	
	Message *telnet = (Message *)arg;
	char *Telnet_strip = new char[sizeof(TelnetHeader)];
	Telnet_strip = (telnet)->msgStripHdr(sizeof(TelnetHeader));
	TelnetHeader *Telnet_stripped = new TelnetHeader;
	Telnet_stripped =(TelnetHeader *)Telnet_strip;
	char bufy[140];
	bzero(bufy,140);
    (telnet)->msgFlat(bufy);
    bufy[(telnet)->msgLen ( )] = '\n';
	cout << "Message Received by Telnet is: " << bufy << endl;
	deliver_Telnet_counter++;
	cout<<"Telnet received " << deliver_Telnet_counter << " messages"<< endl;
	IN_sock_RecvFunc(arg);	
 }

void deliver_RDP(Message *arg)
 {
	cout<<"Inside deliver_RDP "<<endl;	
	Message *rdp = (Message *)arg;
	char *RDP_strip = new char[sizeof(RDPHeader)];
	RDP_strip = (rdp)->msgStripHdr(sizeof(RDPHeader));
	RDPHeader *RDP_stripped = new RDPHeader;
	RDP_stripped =(RDPHeader *)RDP_strip;
	char bufy[140];
	bzero(bufy,140);
	(rdp)->msgFlat(bufy);
    bufy[(rdp)->msgLen ( )] = '\n';
    cout << "Message Received by RDP is: " << bufy << endl;	
	deliver_RDP_counter++;
	cout<<"RDP received " << deliver_RDP_counter << " messages"<< endl;
	IN_sock_RecvFunc(arg);		
 }

void deliver_DNS(Message *arg)
 {
	cout<<"Inside deliver_DNS "<<endl;	
	Message *dns = (Message *)arg;
	char *DNS_strip = new char[sizeof(DNSHeader)];
	DNS_strip = (dns)->msgStripHdr(sizeof(DNSHeader));
	DNSHeader *DNS_stripped = new DNSHeader;
	DNS_stripped =(DNSHeader *)DNS_strip;
	char bufy[140];
	bzero(bufy,140);
	(dns)->msgFlat(bufy);
    bufy[(dns)->msgLen ( )] = '\n';
    cout << "Message Received by DNS is: " << bufy << endl;	
	deliver_DNS_counter++;
	cout<<"DNS Client received " << deliver_DNS_counter << " messages"<< endl;
	IN_sock_RecvFunc(arg);	
 }


int main()
 {
	cout<<"Inside Main Function "<<endl;
	// create a UDP IN socket 
	if((UDP_IN_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	 {
		perror("cannot create socket\n");
		return 0;
	 }

	// bind the UDP IN socket to any valid IP address and self-chosen port 
	memset((char *)&in_addr, 0, sizeof(in_addr));
	in_addr.sin_family = AF_INET;
	in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	in_addr.sin_port = htons(0);

	//binding the UDP IN socket on the server
	if(bind(UDP_IN_sock, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0)
	 {
		perror("bind failed");
		return 0;
	 }
	//get IN port number
	if(getsockname(UDP_IN_sock, (struct sockaddr *)&in_addr, &in_addrlen) == 0 && in_addr.sin_family == AF_INET)
	 {
		UDP_IN_port = ntohs(in_addr.sin_port);
		printf("IN is on portnumber: %d\n", UDP_IN_port);
	 }

	/**************************************************************************************/
	//create a UDP OUT socket 
	if((UDP_OUT_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	 {
		perror("cannot create socket\n");
		return 0;
	 }

	//bind the UDP OUT socket to any valid IP address and self-chosen port 
	memset((char *)&out_addr, 0, sizeof(out_addr));
	out_addr.sin_family = AF_INET;
	out_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	out_addr.sin_port = htons(0);

	//binding the UDP IN socket on the server
	if(bind(UDP_OUT_sock, (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0)
	 {
		perror("bind failed");
		return 0;
	 }
	//get OUT port number
	if(getsockname(UDP_OUT_sock, (struct sockaddr *)&out_addr, &out_addrlen) == 0 && out_addr.sin_family == AF_INET && out_addrlen == sizeof(out_addr))
	 {
		UDP_OUT_port = ntohs(out_addr.sin_port);
		printf("OUT is on portnumber: %d\n", UDP_OUT_port);
	 }

	/****************************************************************************************/

	printf("Enter the port you want to recieve from(OUT port of the otherside): ");
	scanf("%d", &port1);
	printf("Enter the port you want to send to(IN port of the otherside): ");
	scanf("%d", &port2);

	

/*************************Create Thread to receive Message from the socket********************************************/
	cout<<"Creating the IN_sock_RecvThread"<<endl;
	ret = pthread_create(&IN_sock_RecvThread, NULL, &IN_sock_RecvFunc, NULL);
	if(ret != 0) 
	{
		   printf("Error:IN_sock_RecvThread pthread_create() failed\n");
		   exit(EXIT_FAILURE);
	}
	sleep(1);

	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port2);

/*************************Create Thread to receive Message from application to FTP***********************************/
	cout<<"Creating FTPapp thread .."<<endl;
	ret = pthread_create(&FTPapp_RecvThread, NULL, &FTPapp, NULL);
	if(ret != 0) 
	{
		   printf("Error:IN_sock_RecvThread pthread_create() failed\n");
		   exit(EXIT_FAILURE);
	}
	sleep(1);

/*************************Create Thread to receive Message from application to Telnet*******************************/
	cout<<"Creating Telnetapp thread .."<<endl;
	ret = pthread_create(&Telnetapp_RecvThread, NULL, &Telnetapp, NULL);
	if(ret != 0) 
	{
		   printf("Error:IN_sock_RecvThread pthread_create() failed\n");
		   exit(EXIT_FAILURE);
	}
	sleep(1); 

/*************************Create Thread to receive Message from application to RDP***********************************/
	ret = pthread_create(&RDPapp_RecvThread, NULL, &RDPapp, NULL);
	if(ret != 0) 
	{
		   printf("Error:IN_sock_RecvThread pthread_create() failed\n");
		   exit(EXIT_FAILURE);
	}
	sleep(1); 

/*************************Create Thread to receive Message from application to DNS Client**************************/
	ret = pthread_create(&DNSapp_RecvThread, NULL, &DNSapp, NULL);
	if(ret != 0) 
	{
		   printf("Error:IN_sock_RecvThread pthread_create() failed\n");
		   exit(EXIT_FAILURE);
	}
	sleep(1);  
	while(1)
	//for(int i=0;i<8;i++)
	{


	}
	
 }

