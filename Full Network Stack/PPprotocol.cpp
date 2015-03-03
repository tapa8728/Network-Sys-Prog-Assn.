#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include<iostream>
#include <sys/time.h>

using namespace std;

#include "message.h"

#define BUFSIZE 2048
#define datalen 10

//structure of the header
struct EthernetHeader	//1
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[8];
	int msglen; 	//length of data without the header
};

struct IPHeader	//2
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[12];
	int msglen; 	//length of data without the header
};

struct TCPHeader	//3
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[4];
	int msglen; 	//length of data without the header
};

struct UDPHeader	//4
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[4];
	int msglen; 	//length of data without the header
};

struct FTPHeader	//5
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[8];
	int msglen; 	//length of data without the header
};

struct TelnetHeader	//6
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[8];
	int msglen; 	//length of data without the header
};

struct RDPHeader	//7
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[12];
	int msglen; 	//length of data without the header
};

struct DNSCHeader	//8
{
	int hlp;	//protocol ID of higher-level protocol
	char oi[8];
	int msglen; 	//length of data without the header
};

struct unit
{
	int proto_id;
	Message *mp;
};

struct sockaddr_in in_addr;	/* IN address */
struct sockaddr_in out_addr;	/* OUT address */
socklen_t out_addrlen = sizeof(out_addr);	/* length of addresses */
socklen_t in_addrlen = sizeof(in_addr);
int UDP_IN_sock, UDP_OUT_sock;	/* our socket */
int UDP_IN_port;	/*IN port*/
int UDP_OUT_port; 	/*OUT port*/

char buf[BUFSIZE];	/* receive buffer */
char data[100];


int sendEthernetPipe[2];	//Array integer for calling - Send Ethernet pipe
int sendIPPipe[2];	//Array integer for calling - Send IP pipe
int sendTCPPipe[2];	//Array integer for calling - Send TCP pipe
int sendUDPPipe[2];	//Array integer for calling - Send UDP pipe
int sendFTPPipe[2];	//Array integer for calling - Send FTP pipe
int sendTelnetPipe[2];	//Array integer for calling - Send Telnet pipe
int sendRDPPipe[2];	//Array integer for calling - Send RDP pipe
int sendDNSCPipe[2];	//Array integer for calling - Send DNS Client pipe

int recvEthernetPipe[2];	//Array integer for calling - Recieve Ethernet pipe
int recvIPPipe[2];	//Array integer for calling - Recieve IP pipe
int recvTCPPipe[2];	//Array integer for calling - Recieve TCP pipe
int recvUDPPipe[2];	//Array integer for calling - Recieve UDP pipe
int recvFTPPipe[2];	//Array integer for calling - Recieve FTP pipe
int recvTelnetPipe[2];	//Array integer for calling - Recieve Telnet pipe
int recvRDPPipe[2];	//Array integer for calling - Recieve RDP pipe
int recvDNSCPipe[2];	//Array integer for calling - Recieve DNS Client pipe

int user,port1, port2;
struct sockaddr_in remaddr;
//threads for send-pipe interaction
pthread_t DNSCSendThread, RDPSendThread, TelnetSendThread, FTPSendThread, UDPSendThread, TCPSendThread, IPSendThread, EthernetSendThread;
//threads for recieve-pipe interaction
pthread_t EthernetRecvThread, IPRecvThread, TCPRecvThread, UDPRecvThread, FTPRecvThread, TelnetRecvThread, RDPRecvThread, DNSCRecvThread;
//application threads
pthread_t AppFTPThread, AppTelnetThread, AppDNSCThread, AppRDPThread;
int ret;

pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;

int count_ftp_send=0;
int count_telnet_send=0;
int count_rdp_send=0;
int count_dnsc_send=0;
int count_ftp_recv=0;
int count_telnet_recv=0;
int count_rdp_recv=0;
int count_dnsc_recv=0;

class network
{

	public:
	network();	//constructor
};
	//contructor
	network:: network()
	{
	cout<<"this constructor does nothing"<<endl;	
	}
	/***********************************************************************************************************/
	/*	APPLICATION THREADS	     	   */	
	/***********************************************************************************************************/
	/*	FTP APPLICATION Thread
	AppFTPFunc- Write 100 messages to sendFTPPipe */
	/*AppFTPFunc*/
	void *AppFTPFunc(void *arg)
	{
		for(int i=0; i<100; i++)
		{
			//wait for the message on the recvEthernetPipe	
			//cout<<"inside AppFTPFunc"<<endl;
			char data1[100] = "ffffffffffffffffffffffffffffffffffffffff";
			char *b1 = data1;
			Message *m = new Message(b1, 100);
			char test1[140];
			m->msgFlat(test1);
			test1[m->msgLen ( )] = '\n';
			//cout << "Message written to FTP is: " << test1 << endl;

			//create the message struct
			unit u;
			u.proto_id=5;
			u.mp=m;
			
			count_ftp_send++;
			if(count_ftp_send>94)
			{
				cout<<"FTP sent: "<<count_ftp_send<<endl;
			}
			write(sendFTPPipe[1], (char*)&u, sizeof(unit));	//write on the app layer pipes
			usleep(3000);
		}
	}
	/***********************************************************************************************************/
	/*	Telnet APPLICATION Thread
	AppTelnetFunc- Write 100 messages to sendTelnetPipe */
	/*AppTelnetFunc*/
	void *AppTelnetFunc(void *arg)
	{
		for(int i=0; i<100; i++)
		{
			//wait for the message on the recvEthernetPipe	
			//cout<<"inside AppTelnetFunc"<<endl;
			char data1[100] = "ttttttttttttttttttttttttttttttttttttttttttttttttt";
			char *b1 = data1;
			Message *m = new Message(b1, 100);
			char test2[140];
			m->msgFlat(test2);
			test2[m->msgLen ( )] = '\n';
			//cout << "Message written to FTP is: " << test1 << endl;

			//create the message struct
			unit u;
			u.proto_id=6;
			u.mp=m;
			count_telnet_send++;
			if(count_telnet_send>94)
			{
				cout<<"Telnet sent: "<<count_telnet_send<<endl;
			}
			write(sendTelnetPipe[1], (char*)&u, sizeof(unit));	//write on the app layer pipes
			usleep(3000);
		}
	}
	/***********************************************************************************************************/
	/*	RDP APPLICATION Thread
	AppRDPFunc- Write 100 messages to sendRDPPipe */
	/*AppRDPFunc*/
	void *AppRDPFunc(void *arg)
	{
		for(int i=0; i<100; i++)
		{
			//wait for the message on the recvEthernetPipe	
			//cout<<"inside AppRDPFunc"<<endl;
			char data1[100] = "rrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr";
			char *b1 = data1;
			Message *m = new Message(b1, 100);
			char test3[140];
			m->msgFlat(test3);
			test3[m->msgLen ( )] = '\n';
			//cout << "Message written to RDP is: " << test3 << endl;

			//create the message struct
			unit u;
			u.proto_id=7;
			u.mp=m;

			count_rdp_send++;
			if(count_rdp_send>94)
			{
				cout<<"RDP sent: "<<count_rdp_send<<endl;
			}
			write(sendRDPPipe[1], (char*)&u, sizeof(unit));	//write on the app layer pipes
			usleep(3000);
		}
	}
	/***********************************************************************************************************/
	/*	DNS Client APPLICATION Thread
	AppDNSCFunc- Write 100 messages to sendDNSCPipe */
	/*AppDNSCFunc*/
	void *AppDNSCFunc(void *arg)
	{
		for(int i=0; i<100; i++)
		{
			//wait for the message on the recvEthernetPipe	
			//cout<<"inside AppDNSCFunc"<<endl;
			char data1[100] = "dddddddddddddddddddddddddddddddddddddddddddddddd";
			char *b1 = data1;
			Message *m = new Message(b1, 100);
			char test4[140];
			m->msgFlat(test4);
			test4[m->msgLen ( )] = '\n';
			//cout << "Message written to FTP is: " << test1 << endl;

			//create the message struct
			unit u;
			u.proto_id=8;
			u.mp=m;

			count_dnsc_send++;
			if(count_dnsc_send>94)
			{
				cout<<"DNS sent: "<<count_dnsc_send<<endl;
			}
			write(sendDNSCPipe[1], (char*)&u, sizeof(unit));	//write on the app layer pipes
			usleep(3000);
		}
	}
	/******************************************************************************************************/
	/*	RECIEVING FUNCTIONS     	      */
	/******************************************************************************************************/
	/*	ETHERNET PROTOCOL - Recieve
	EthernetRecvThread-continuously wait on UDP socket to recv(if recieved strip header, demultiplex, 
	write to recvIPPipe	*/
	
	/*read_from_UDP*/
	void *read_from_UDP(void *arg)
	{
		//EthernetRecvThread thread continously reads from UDP socket
		printf("\n*****Host waiting on port %d*****\n", UDP_IN_port);
		//cout<<"inside read_from_UDP "<<endl;	
		
		socklen_t remlen = sizeof(remaddr);
		while (1)
		{
			
			int recvlen = recvfrom(UDP_IN_sock, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &remlen);
			if (recvlen > 0)
			{
				buf[recvlen] = '\0';
				//printf("SOCKET# received message: %s (%d bytes)\n", buf, recvlen);
				//write to recvEthernetPipe
				Message *m = new Message(buf, sizeof(buf));
				
				//strip header	
				char *strip  = new char[sizeof(struct EthernetHeader)];
				strip = m->msgStripHdr(sizeof(struct EthernetHeader));
				struct EthernetHeader *stripped = (struct EthernetHeader *)strip;
				//cout<<"---Ethernet Stripped hlp "<< stripped->hlp <<endl;
				//cout<<"---Ethernet Stripped len "<< stripped->msglen <<endl;
				
				unit u;
				u.mp=m;	
				//demultiplex
				//write to recvIPPipe
				write(recvIPPipe[1], (char*)&u, sizeof(unit));

				//memset(&buf[0], 0, sizeof(buf));
			}
		}
	}
	/******************************************************************************************************/
	/*	IP PROTOCOL - Recieve
	IPRecvThread-continuously wait on recvIPPipe to read(if read, strip header, demultiplex, 
	write to recvIPPipe	*/
	/*IPRecvFunc*/
	void *IPRecvFunc(void *arg)
	{
		//wait for the message on the recvEthernetPipe	
		//cout<<"inside IPRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u=new unit;
			if ((n = read(recvIPPipe[0], (unit*)u, sizeof(unit))) > 0)
			{
				//strip header
				unit u1;
				u1.mp=u->mp;
	
				char *ip_strip = new char[sizeof(IPHeader)];
				ip_strip = (u1.mp)->msgStripHdr(sizeof(IPHeader));
				IPHeader *ip_stripped = new IPHeader;
				ip_stripped =(IPHeader *)ip_strip;
				//cout<<"----IP Stripped hlp "<<ip_stripped->hlp<<endl;
				//cout<<"------IP Stripped len "<<ip_stripped->msglen<<endl;	
	
				//demultiplex
				if(ip_stripped->hlp == 3)
				{
					write(recvTCPPipe[1], (char*)&u1, sizeof(unit));
					//printf("Successfully written to recvTCPPipe\n");
				}
				else if(ip_stripped->hlp == 4)
				{
					write(recvUDPPipe[1], (char*)&u1, sizeof(unit));
					//printf("Successfully written to recvUDPPipe\n");
				}
			}	
			else
				perror("read");
		}
	}
	/***********************************************************************************************************/
	/*	TCP PROTOCOL - Recieve
	TCPRecvFunc-continuously wait on recvIPPipe to read(if read, strip header, demultiplex, write to
	recvFTPPipe or recvTelnetPipe based on the hlp field	*/
	/*TCPRecvFunc*/
	void *TCPRecvFunc(void *arg)
	{
		//wait for the message on the recvTCPPipe	
		//cout<<"inside TCPRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u_tcp=new unit;
			if ((n = read(recvTCPPipe[0], (unit*)u_tcp, sizeof(unit))) > 0)
			{
				//strip header
				unit u1_tcp;
				u1_tcp.proto_id=u_tcp->proto_id;
				u1_tcp.mp=u_tcp->mp;
	
				char *tcp_strip = new char[sizeof(TCPHeader)];
				tcp_strip = (u1_tcp.mp)->msgStripHdr(sizeof(TCPHeader));
				TCPHeader *tcp_stripped = new TCPHeader;
				tcp_stripped =(TCPHeader *)tcp_strip;
				//cout<<"----TCP Stripped hlp "<<tcp_stripped->hlp<<endl;
				//cout<<"----TCP Stripped len "<<tcp_stripped->msglen<<endl;	
	
				//demultiplex
				if(tcp_stripped->hlp == 5)	//FTP
				{
					write(recvFTPPipe[1], (char*)&u1_tcp, sizeof(unit));
					//printf("Successfully written to recvFTPPipe\n");
				}
				else if(tcp_stripped->hlp == 6)	//Telnet
				{
					write(recvTelnetPipe[1], (char*)&u1_tcp, sizeof(unit));
					//printf("Successfully written to recvTelnetPipe\n");
				}
			}	
			else
				perror("read");
		}
	}
	/***********************************************************************************************************/
	/*	UDP PROTOCOL - Recieve
	UDPRecvFunc-continuously wait on recvUDPPipe to read(if read, strip header, demultiplex, write to
	recvRDPPipe or recvDNSCPipe based on the hlp field	*/
	/*UDPRecvFunc*/
	void *UDPRecvFunc(void *arg)
	{
		//wait for the message on the recvUDPPipe	
		//cout<<"inside UDPRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u_udp=new unit;
			if ((n = read(recvUDPPipe[0], (unit*)u_udp, sizeof(unit))) > 0)
			{
				//strip header
				unit u1_udp;
				u1_udp.proto_id=u_udp->proto_id;
				u1_udp.mp=u_udp->mp;
	
				char *udp_strip = new char[sizeof(UDPHeader)];
				udp_strip = (u1_udp.mp)->msgStripHdr(sizeof(UDPHeader));
				UDPHeader *udp_stripped = new UDPHeader;
				udp_stripped =(UDPHeader *)udp_strip;
				//cout<<"----UDP Stripped hlp "<<udp_stripped->hlp<<endl;
	
				//demultiplex
				if(udp_stripped->hlp == 7)	//RDP
				{
					write(recvRDPPipe[1], (char*)&u1_udp, sizeof(unit));
					//printf("Successfully written to recvRDPPipe\n");
				}
				else if(udp_stripped->hlp == 8)	//DNS Client
				{
					write(recvDNSCPipe[1], (char*)&u1_udp, sizeof(unit));
					//printf("Successfully written to recvDNSCPipe\n");
				}
			}	
			else
				perror("read");
		}
	}
	/***********************************************************************************************************/
	/*	FTP PROTOCOL - Recieve
	FTPRecvFunc-continuously wait on recvTCPPipe to read(if read, strip header, demultiplex, write to
	recvTCPPipe */
	/*FTPRecvFunc*/
	void *FTPRecvFunc(void *arg)
	{
		//wait for the message on the recvFTPPipe	
		//cout<<"inside FTPRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u_ftp=new unit;
			if ((n = read(recvFTPPipe[0], (unit*)u_ftp, sizeof(unit))) > 0)
			{
				//strip header
				unit u1_ftp;
				u1_ftp.proto_id=u_ftp->proto_id;
				u1_ftp.mp=u_ftp->mp;
	
				char *ftp_strip = new char[sizeof(FTPHeader)];
				ftp_strip = (u1_ftp.mp)->msgStripHdr(sizeof(FTPHeader));
				FTPHeader *ftp_stripped = new FTPHeader;
				ftp_stripped =(FTPHeader *)ftp_strip;
				//cout<<"----FTP Stripped hlp "<<ftp_stripped->hlp<<endl;
	
				//display the output message
				char bufy[200];
				(u1_ftp.mp)->msgFlat(bufy);
			    	bufy[(u1_ftp.mp)->msgLen ( )] = '\n';
				count_ftp_recv++;
				if(count_ftp_recv>94)
				{
					cout<<"FTP Received: "<<count_ftp_recv<<endl;
			  		//cout << "Message Recieved from FTP: " << bufy << endl;
				}	
			}	
			else
				perror("read");
		}
	}
	/***********************************************************************************************************/
	/*	Telnet PROTOCOL - Recieve
	TelnetRecvFunc-continuously wait on recvTCPPipe to read(if read, strip header, demultiplex, write to
	recvTelnetPipe */
	/*TelnetRecvFunc*/
	void *TelnetRecvFunc(void *arg)
	{
		//wait for the message on the recvTelnetPipe	
		//cout<<"inside TelnetRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u_telnet=new unit;
			if ((n = read(recvTelnetPipe[0], (unit*)u_telnet, sizeof(unit))) > 0)
			{
				//strip header
				unit u1_telnet;
				u1_telnet.proto_id=u_telnet->proto_id;
				u1_telnet.mp=u_telnet->mp;
	
				char *telnet_strip = new char[sizeof(TelnetHeader)];
				telnet_strip = (u1_telnet.mp)->msgStripHdr(sizeof(TelnetHeader));
				TelnetHeader *telnet_stripped = new TelnetHeader;
				telnet_stripped =(TelnetHeader *)telnet_strip;
				//cout<<"----Telnet Stripped hlp "<<telnet_stripped->hlp<<endl;
	
				//just display the output message
				char bufy[140];
				(u1_telnet.mp)->msgFlat(bufy);
			    	bufy[(u1_telnet.mp)->msgLen ( )] = '\n';
				count_telnet_recv++;
				if(count_telnet_recv>94)
				{
					cout<<"Telnet Received: "<<count_telnet_recv<<endl;
			  		//cout << "Message Recieved from Telnet: " << bufy << endl;	
				}
			}	
			else
				perror("read");
		}
	}

	/***********************************************************************************************************/
	/*	RDP PROTOCOL - Recieve
	RDPRecvFunc-continuously wait on recvUDPPipe to read(if read, strip header, demultiplex, write to
	recvRDPPipe */
	/*RDPRecvFunc*/
	void *RDPRecvFunc(void *arg)
	{
		//wait for the message on the recvRDPPipe	
		//cout<<"inside RDPRecvFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u_rdp=new unit;
			if ((n = read(recvRDPPipe[0], (unit*)u_rdp, sizeof(unit))) > 0)
			{
				//strip header
				unit u1_rdp;
				u1_rdp.proto_id=u_rdp->proto_id;
				u1_rdp.mp=u_rdp->mp;
	
				char *rdp_strip = new char[sizeof(RDPHeader)];
				rdp_strip = (u1_rdp.mp)->msgStripHdr(sizeof(RDPHeader));
				RDPHeader *rdp_stripped = new RDPHeader;
				rdp_stripped =(RDPHeader *)rdp_strip;
				//cout<<"----RDP Stripped hlp "<<rdp_stripped->hlp<<endl;
	
				//just display the output message
				char bufy[140];
				(u1_rdp.mp)->msgFlat(bufy);
			    	bufy[(u1_rdp.mp)->msgLen ( )] = '\n';
				count_rdp_recv++;
				if(count_rdp_recv>94)
				{
					cout<<"RDP Received: "<<count_rdp_recv<<endl;
			  		//cout << "Message Recieved from RDP: " << bufy << endl;
				}	
			}	
			else
				perror("read");
		}
	}
	/***********************************************************************************************************/
	/*	DNS Client PROTOCOL - Recieve
	DNSCRecvFunc-continuously wait on recvUDPPipe to read(if read, strip header, demultiplex, write to
	recvDNSCPipe */
	/*DNSCRecvFunc*/
	void *DNSCRecvFunc(void *arg)
	{
	//wait for the message on the recvDNSCPipe	
	//cout<<"inside DNSCRecvFunc"<<endl;	
	int n;	
	while(1)
	{
		unit *u_dnsc=new unit;
		if ((n = read(recvDNSCPipe[0], (unit*)u_dnsc, sizeof(unit))) > 0)
		{
			//strip header
			unit u1_dnsc;
			u1_dnsc.proto_id=u_dnsc->proto_id;
			u1_dnsc.mp=u_dnsc->mp;
	
			char *dnsc_strip = new char[sizeof(DNSCHeader)];
			dnsc_strip = (u1_dnsc.mp)->msgStripHdr(sizeof(DNSCHeader));
			DNSCHeader *dnsc_stripped = new DNSCHeader;
			dnsc_stripped =(DNSCHeader *)dnsc_strip;
			//cout<<"----DNSC Stripped hlp "<<dnsc_stripped->hlp<<endl;
	
			//just display the output message
			char bufy[200];
			(u1_dnsc.mp)->msgFlat(bufy);
		    	bufy[(u1_dnsc.mp)->msgLen ( )] = '\n';
			count_dnsc_recv++;
			if(count_dnsc_recv>94)
			{
				cout<<"DNS Received: "<<count_dnsc_recv<<endl;
		  		//cout << "Message Recieved from DNSC: " << bufy << endl;	
			}
		}	
		else
			perror("read");
		}
	}
	/******************************************************************************************************/
	/*	SENDING FUNCTIONS     	      */
	/******************************************************************************************************/
	/*********************** APPLICATION LAYER - FTP, Telnet, RDP, DNS Client *****************************/
	/******************************************************************************************************/
	/*	DNS Client Protocol - SEND
	DNSCSendThread - call function DNSCSendFunc(wait on sendDNSCPipe, build DNSC header and add it to message,
	write it to sendUDPPipe  */
	/*DNSCSendFunc*/
	void *DNSCSendFunc(void *arg)	//write to UDP
	{
		//wait for message on sendFTPPipe
		//cout<<"Inside DNSCSendFunc "<<endl;
		int t;
		while(1)
		{
			unit *u_DNSC = new unit;
			if ((t = read(sendDNSCPipe[0], (unit*)u_DNSC, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock2);	
				unit u1_DNSC;
				u1_DNSC.proto_id = 8;
				u1_DNSC.mp=u_DNSC->mp;
				//build DNSC header
				DNSCHeader *dnsch = new DNSCHeader;
				dnsch->hlp=u_DNSC->proto_id;
				snprintf(dnsch->oi, 8, "ETHERNEsassassssss");
				dnsch->msglen=(u1_DNSC.mp)->msgLen();
				//cout<<"DNSC HLP is :"<<dnsch->hlp<<endl;
				//cout<<"Size of DNSC header is... "<<sizeof(DNSCHeader)<<endl;
				//cout<<"Length before adding DNSC Header ..."<<(u1_DNSC.mp)->msgLen()<<endl;
				//Add DNSC header
				(u1_DNSC.mp)->msgAddHdr((char *)dnsch, sizeof(DNSCHeader));
				//cout<<"**Length after adding RDP Header ..."<<(u1_DNSC.mp)->msgLen()<<endl;
				//write it to sendUDPPipe
				write(sendUDPPipe[1], (char*)&u1_DNSC, sizeof(unit));
				//printf("Successfully written to sendUDPPipe\n");
				pthread_mutex_unlock(&lock2);
			}
		}
	}
	/******************************************************************************************************/
	/*	RDP Protocol - SEND
	RDPSendThread - call function RDPSendFunc(wait on sendRDPPipe, build RDP header and add it to message,
	write it to sendUDPPipe  */
	
	/*RDPSendFunc*/
	void *RDPSendFunc(void *arg)	//write to UDP
	{
		//wait for message on sendFTPPipe
		//cout<<"Inside RDPSendFunc "<<endl;
		int g;
		while(1)
		{
			unit *u_RDP = new unit;
			if ((g = read(sendRDPPipe[0], (unit*)u_RDP, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock2);
				unit u1_RDP;
				u1_RDP.proto_id = 7;
				u1_RDP.mp=u_RDP->mp;
				//build RDP header
				RDPHeader *rdph = new RDPHeader;
				rdph->hlp=u_RDP->proto_id;
				snprintf(rdph->oi, 12, "ETHERNEsassassssss");
				rdph->msglen=(u1_RDP.mp)->msgLen();
				//cout<<"RDP HLP is :"<<rdph->hlp<<endl;
				//cout<<"Size of RDP header is... "<<sizeof(RDPHeader)<<endl;
				//cout<<"Length before adding RDP Header ..."<<(u1_RDP.mp)->msgLen()<<endl;
				//Add RDP header
				(u1_RDP.mp)->msgAddHdr((char *)rdph, sizeof(RDPHeader));
				//cout<<"**Length after adding RDP Header ..."<<(u1_RDP.mp)->msgLen()<<endl;
				//write it to sendUDPPipe
				write(sendUDPPipe[1], (char*)&u1_RDP, sizeof(unit));
				//printf("Successfully written to sendUDPPipe\n");
				pthread_mutex_unlock(&lock2);
			}
		}
	}
	/******************************************************************************************************/
	/*	Telnet Protocol - SEND
	TelnetSendThread - call function TelnetSendFunc(wait on sendTelnetPipe, build Telnet header and add it to message,
	write it to sendTCPPipe  */
	
	/*TelnetSendFunc*/
	void *TelnetSendFunc(void *arg)	//write to TCP
	{
		//wait for message on sendFTPPipe
		//cout<<"Inside TelnetSendFunc "<<endl;
		int j;
		while(1)
		{
			unit *u_telnet = new unit;
			if ((j = read(sendTelnetPipe[0], (unit*)u_telnet, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock1);								
				//build Telnet header
				unit u1_telnet;
				u1_telnet.proto_id = 6;
				u1_telnet.mp=u_telnet->mp;
				TelnetHeader *th = new TelnetHeader;
				th->hlp=u_telnet->proto_id;
				snprintf(th->oi, 8, "ETHERNEsassassssss");
				th->msglen=(u1_telnet.mp)->msgLen();
				//cout<<"Telnet HLP is :"<<th->hlp<<endl;
				//cout<<"Size of Telnet header is... "<<sizeof(TelnetHeader)<<endl;
				//cout<<"Length before adding Telnet Header ..."<<(u1_telnet.mp)->msgLen()<<endl;
				//Add Telnet header
				(u1_telnet.mp)->msgAddHdr((char *)th, sizeof(TelnetHeader));
				//cout<<"**Length after adding Telnet Header ..."<<(u1_telnet.mp)->msgLen()<<endl;
				//write it to sendTCPPipe
				write(sendTCPPipe[1], (char*)&u1_telnet, sizeof(unit));
				//printf("Successfully written to sendTCPPipe\n");
				pthread_mutex_unlock(&lock1);
			}
		}
	}	
	/******************************************************************************************************/
	/*	FTP Protocol - SEND
	FTPSendThread - call function FTPSendFunc(wait on sendFTPPipe, build FTP header and add it to message,
	write it to sendTCPPipe  */
	
	/*FTPSendFunc*/
	void *FTPSendFunc(void *arg)	//write to TCP
	{
		//wait for message on sendFTPPipe
		//cout<<"Inside FTPPSendFunc "<<endl;
		int k;
		while(1)
		{
			unit *u_ftp = new unit;
			if ((k = read(sendFTPPipe[0], (unit*)u_ftp, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock1);	
				unit u1_ftp;
				u1_ftp.proto_id = 5;
				u1_ftp.mp=u_ftp->mp;
				//build FTP header
				FTPHeader *ftph = new FTPHeader;
				ftph->hlp=u_ftp->proto_id;
				snprintf(ftph->oi, 12, "ETHERNEsassassssss");
				ftph->msglen=(u1_ftp.mp)->msgLen();
				//cout<<"FTP HLP is :"<<ftph->hlp<<endl;
				//cout<<"Size of FTP header is... "<<sizeof(FTPHeader)<<endl;
				//cout<<"Length before adding FTP Header ..."<<(u1_ftp.mp)->msgLen()<<endl;
				//Add TCP header
				(u1_ftp.mp)->msgAddHdr((char *)ftph, sizeof(FTPHeader));
				//cout<<"**Length after adding FTP Header ..."<<(u1_ftp.mp)->msgLen()<<endl;
				//write it to sendTCPPipe
				write(sendTCPPipe[1], (char*)&u1_ftp, sizeof(unit));
				//printf("Successfully written to sendTCPPipe\n");
				pthread_mutex_unlock(&lock1);
			}
		}
	}
	/******************************************************************************************************/
	/*********************** NETWORK LAYER - TCP, UDP *****************************/	
	/******************************************************************************************************/
	/*	UDP Protocol - SEND
	UDPSendThread - call function UDPSendFunc(wait on sendUDPPipe, build UDP header and add it to message,
	write it to sendIPPipe  */
	
	/*UDPSendFunc*/
	void *UDPSendFunc(void *arg)	//write to IP
	{
		//wait for message on sendUDPPipe
		//cout<<"Inside UDPSendFunc "<<endl;
		int l;
		while(1)
		{
			unit *u_udp = new unit;
			if ((l = read(sendUDPPipe[0], (unit*)u_udp, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock3);	
				unit u1_udp;
				u1_udp.proto_id = 4;
				u1_udp.mp=u_udp->mp;
				//build UDP header	
				UDPHeader *udph = new UDPHeader;
				udph->hlp=u_udp->proto_id;
				udph->msglen=(u1_udp.mp)->msgLen();
				snprintf(udph->oi, 4, "ETHERNEsassassssss");
				//cout<<"UDP HLP is :"<<udph->hlp<<endl;	
				//cout<<"Size of UDP header is "<<sizeof(UDPHeader)<<endl;
				//cout<<"Length before adding UDP Header ..."<<(u1_udp.mp)->msgLen()<<endl;
				//Add UDP header
				(u1_udp.mp)->msgAddHdr((char *)udph, sizeof(UDPHeader));
				//cout<<"**Length after adding UDP Header ..."<<(u1_udp.mp)->msgLen()<<endl;
				//write it to sendIPPipe
				write(sendIPPipe[1], (char*)&u1_udp, sizeof(unit));
				//printf("Successfully written to sendIPPipe\n");
				pthread_mutex_unlock(&lock3);
	
			}
		}
	}	
	/******************************************************************************************************/
	/*	TCP Protocol - SEND
	TCPSendThread - call function TCPSendFunc(wait on sendFTPPipe, build TCP header and add it to message,
	write it to sendIPPipe  */
	
	/*TCPSendFunc*/
	void *TCPSendFunc(void *arg)	//write to IP
	{
		//wait for message on sendFTPPipe
		//cout<<"Inside TCPSendFunc "<<endl;
		int l;
		while(1)
		{
			unit *u_tcp = new unit;
			if ((l = read(sendTCPPipe[0], (unit*)u_tcp, sizeof(unit))) > 0)
			{
				pthread_mutex_lock(&lock3);			
				//build TCP header
				unit u1_tcp;
				u1_tcp.proto_id = 3;
				u1_tcp.mp=u_tcp->mp;
				TCPHeader *tcph = new TCPHeader;
				tcph->hlp=u_tcp->proto_id;
				snprintf(tcph->oi, 4, "ETHERNEsassassssss");
				tcph->msglen=(u1_tcp.mp)->msgLen();
				//cout<<"TCP HLP is :"<<tcph->hlp<<endl;	
				//cout<<"Size of TCP header is "<<sizeof(TCPHeader)<<endl;
				//cout<<"Length before adding TCP Header ..."<<(u1_tcp.mp)->msgLen()<<endl;
				//Add TCP header
				(u1_tcp.mp)->msgAddHdr((char *)tcph, sizeof(TCPHeader));
				//cout<<"**Length after adding TCP Header ..."<<(u1_tcp.mp)->msgLen()<<endl;
				//write it to sendIPPipe
				write(sendIPPipe[1], (char*)&u1_tcp, sizeof(unit));
				//printf("Successfully written to sendIPPipe\n");
				pthread_mutex_unlock(&lock3);
	
			}
		}
	}	
	
	/*****************************************************************************************************
	/*	IP Protocol - SEND
	IPSendThread-call function IPSendFunc(wait on sendIPPipe, build the IP header and add it to message, write it to 
	sendEthernetPipe	*/	
	/*IPSendFunc*/
	void *IPSendFunc(void *arg)
	{
		//wait for the message on the sendIPPipe	
		//cout<<"inside IPSendFunc"<<endl;	
		int n;	
		while(1)
		{
			unit *u=new unit;
			if ((n = read(sendIPPipe[0], (unit*)u, sizeof(unit))) > 0)
			{
				unit u1;
				u1.proto_id=2;	//add your own protocol ID
				u1.mp=u->mp;	
				//Build IP header
				IPHeader *iph = new IPHeader;
				iph->hlp=u->proto_id;	//higher level Protoccol ID
				snprintf(iph->oi, 12, "ETHERNEsassassssss");
				iph->msglen=(u1.mp)->msgLen();
				//cout<<"IP HLP is :"<<iph->hlp<<endl;
				//cout<<"Size of Ip header is "<<sizeof(IPHeader)<<endl;
				//cout<<"Length before adding IP Header ..."<<(u1.mp)->msgLen()<<endl;
				//add header
				(u1.mp)->msgAddHdr((char *)iph, sizeof(IPHeader));
				//cout<<"**Length after adding IP Header ..."<<(u1.mp)->msgLen()<<endl;

				//write it to sendEthernetPipe
				write(sendEthernetPipe[1], (char*)&u1, sizeof(unit));
				//printf("Successfully written to sendEthernetPipe\n");
			}	
			else
				perror("read");
		}
	}
	/*****************************************************************************************************/
	/* 	Ethernet Protocol - SEND
	EthernetSendThread-call EthernetSendFunc(continously wait on sendEthernetPipe, call write_UDP_socket)
	write_UDP_socket-Build Header, Add Header, use sendTo and write to socket */
	/*EthernetSendFunc*/
	void *EthernetSendFunc(void *arg)
	{
		//wait for the message on the sendEthernetPipe	
		//cout<<"inside EthernetSendFunc"<<endl;	
		int n;	
		while(1)
		{	
			unit *u=new unit;
			if ((n = read(sendEthernetPipe[0], (unit*)u, sizeof(unit))) > 0)
			{
				//cout<<"read sendEthernetPipe proto_id "<< u->proto_id<<endl;
				unit u1;
				u1.mp = u->mp;
				u1.proto_id=1;	//Ip proto id	
				//Build Ethernet Header
				EthernetHeader *ethh = new EthernetHeader;
				ethh->hlp=1; 	//protocol id of the higher level protocol..IP
				snprintf(ethh->oi, 8, "ETHERNEsassassssss");	
				ethh->msglen=(u1.mp)->msgLen();
				//cout<<"Length before adding Ethernet Header ..."<<(u1.mp)->msgLen()<<endl;
				//cout<<"Size of ethernet header is "<<sizeof(EthernetHeader)<<endl;	
				//Attach Ethernet Header
				(u1.mp)->msgAddHdr((char *)ethh, sizeof(EthernetHeader));
				//cout<<"**Length after adding Ethernet Header ..."<<(u1.mp)->msgLen()<<endl;
				//message flat
				//char *buftest = new char[200];	
				char buftest[200];
				(u1.mp)->msgFlat(buftest);
				buftest[(u1.mp)->msgLen()] = '\n';
				//cout << "Message: " << buftest << endl;	
				//call function write_UDP_socket
				if (sendto(UDP_OUT_sock, buftest, sizeof(buftest), 0, (struct sockaddr *)&remaddr, sizeof(remaddr))==-1) ///size=200
				{ 
					perror("sendto");
					exit(1);
				}
				///printf("Successfully written to UDP socket\n");
			}	
			else
				perror("read");
		}
	}
	/*********************************************************************************************/
	/*	Main function	*/
	/*********************************************************************************************/
	int main()
	{
	/*timing info*/
	struct timeval start,end;
    long mtime,useconds;
	gettimeofday(&start, NULL);
	
	pthread_mutex_init(&lock1, NULL);
	pthread_mutex_init(&lock2, NULL);
	pthread_mutex_init(&lock3, NULL);
	
	cout<<"Main function"<<endl;	
	//Send Pipes	
	pipe(sendEthernetPipe);
	pipe(sendIPPipe);
	pipe(sendTCPPipe);
	pipe(sendUDPPipe);
	pipe(sendFTPPipe);
	pipe(sendTelnetPipe);
	pipe(sendRDPPipe);
	pipe(sendDNSCPipe);
	//Recieve Pipes
	pipe(recvEthernetPipe);
	pipe(recvIPPipe);
	pipe(recvTCPPipe);
	pipe(recvUDPPipe);
	pipe(recvFTPPipe);
	pipe(recvTelnetPipe);
	pipe(recvRDPPipe);
	pipe(recvDNSCPipe);	
	/***************************************************************************
	/* create a UDP IN socket */
	if ((UDP_IN_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
	perror("cannot create socket\n");
	return 0;
	}

	/* bind the UDP IN socket to any valid IP address and self-chosen port */
	memset((char *)&out_addr, 0, sizeof(out_addr));
	in_addr.sin_family = AF_INET;
	in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	in_addr.sin_port = htons(0);

	/*binding the UDP IN socket on the server*/
	if (bind(UDP_IN_sock, (struct sockaddr *)&in_addr, sizeof(in_addr)) < 0)
	{
	perror("bind failed");
	return 0;
	}
	
	/*get IN port number*/
	if(getsockname(UDP_IN_sock, (struct sockaddr *)&in_addr, &in_addrlen) == 0 && in_addr.sin_family == AF_INET)
	{
	    UDP_IN_port = ntohs(in_addr.sin_port);
	    printf("IN is on portnumber: %d\n", UDP_IN_port);
	}
	/**************************************************************************************
	
	/**************************************************************************************
	/* create a UDP OUT socket */
	if ((UDP_OUT_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
	perror("cannot create socket\n");
	return 0;
	}

	/* bind the UDP OUT socket to any valid IP address and self-chosen port */
	memset((char *)&out_addr, 0, sizeof(out_addr));
	out_addr.sin_family = AF_INET;
	out_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	out_addr.sin_port = htons(0);

	/*binding the UDP IN socket on the server*/
	if (bind(UDP_OUT_sock, (struct sockaddr *)&out_addr, sizeof(out_addr)) < 0)
	{
	perror("bind failed");
	return 0;
	}
	
	/*get OUT port number*/
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
	char dat[3] ="ac";	
	char *b1 = dat;
	Message *m = new Message(b1, 100);
	/***********************************************************************************************************/
	/*	ETHERNET PROTOCOL - Recieve
	EthernetRecvThread-continuously wait on UDP socket to recv(if recieved,strip header, demultiplex, write to 
	recvEthernetPipe	*/
	/*EthernetRecvThread*/
	ret = pthread_create(&EthernetRecvThread, NULL, &read_from_UDP, (void*)&port1);
	if(ret != 0) 
	{
	        printf("Error:EthernetRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/***********************************************************************************************************/
	/*	IP PROTOCOL - Recieve
	IPRecvThread-continuously wait on recvEthernetPipe to read(if read, strip header, demultiplex, write to
	recvIPPipe	*/
	/*IPRecvThread*/
	ret = pthread_create(&IPRecvThread, NULL, &IPRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:IPRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/***********************************************************************************************************/
	/*	TCP PROTOCOL - Recieve
	TCPRecvThread-continuously wait on recvIPPipe to read(if read, strip header, demultiplex, write to
	recvFTPPipe or recvTelnetPipe based on the hlp field	*/
	/*TCPRecvThread*/
	ret = pthread_create(&TCPRecvThread, NULL, &TCPRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:TCPRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	
	/***********************************************************************************************************/
	/*	UDP PROTOCOL - Recieve
	UDPRecvThread-continuously wait on recvIPPipe to read(if read, strip header, demultiplex, write to
	recvRDPPipe or recvDNSCPipe based on the hlp field	*/
	/*UDPRecvThread*/
	ret = pthread_create(&UDPRecvThread, NULL, &UDPRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:UDPRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);	
	/***********************************************************************************************************/
	/*	FTP PROTOCOL - Recieve
	FTPRecvThread-continuously wait on recvTCPPipe to read(if read, strip header, demultiplex, write to
	recvFTPPipe */
	/*FTPRecvThread*/
	ret = pthread_create(&FTPRecvThread, NULL, &FTPRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:FTPRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/***********************************************************************************************************/
	/*	Telnet PROTOCOL - Recieve
	TelnetRecvThread-continuously wait on recvTCPPipe to read(if read, strip header, demultiplex, write to
	recvTelnetPipe */
	/*TelnetRecvThread*/
	ret = pthread_create(&TelnetRecvThread, NULL, &TelnetRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:TelnetRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}	
	sleep(1);
	/***********************************************************************************************************/
	/*	RDP PROTOCOL - Recieve
	RDPRecvThread-continuously wait on recvUDPPipe to read(if read, strip header, demultiplex, write to
	recvRDPPipe */
	/*RDPRecvThread*/
	ret = pthread_create(&RDPRecvThread, NULL, &RDPRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:RDPRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1); 
	/***********************************************************************************************************/
	/*	DNS Client PROTOCOL - Recieve
	DNSCRecvThread-continuously wait on recvUDPPipe to read(if read, strip header, demultiplex, write to
	recvDNSCPipe */
	/*DNSCRecvThread*/
	ret = pthread_create(&DNSCRecvThread, NULL, &DNSCRecvFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:DNSCRecvThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	} 
	
	/*user input step*/
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(port2);

	
	/******************************************************************************************************/
	/*	SEND THREADS	     	      */
	/******************************************************************************************************/
	/*	DNS Client Protocol - SEND
	DNSCSendThread - call function DNSCSendFunc(wait on sendDNSCPipe, build DNSC header and add it to message,
	write it to sendUDPPipe  */

	/*DNSCSendThread*/
	ret = pthread_create(&DNSCSendThread, NULL, &DNSCSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: DNSCSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/******************************************************************************************************/
	/*	RDP Protocol - SEND
	RDPSendThread - call function RDPSendFunc(wait on sendRDPPipe, build RDP header and add it to message,
	write it to sendUDPPipe  */

	/*RDPSendThread*/
	ret = pthread_create(&RDPSendThread, NULL, &RDPSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: RDPSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/******************************************************************************************************/
	/*	Telnet Protocol - SEND
	TelnetSendThread - call function TelnetSendFunc(wait on sendTelnetPipe, build Telnet header and add it to message,
	write it to sendTCPPipe  */

	/*TelnetSendThread*/	
	ret = pthread_create(&TelnetSendThread, NULL, &TelnetSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: TelnetSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/******************************************************************************************************/
	/*	FTP Protocol - SEND
	FTPSendThread - call function FTPSendFunc(wait on sendFTPPipe, build FTP header and add it to message,
	write it to sendTCPPipe  */
	/*FTPSendThread*/
	ret = pthread_create(&FTPSendThread, NULL, &FTPSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: FTPSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/******************************************************************************************************/
	/*	TCP Protocol - SEND
	TCPSendThread - call function TCPSendFunc(wait on sendFTPPipe, build TCP header and add it to message,
	write it to sendIPPipe  */
	/*TCPSendThread*/
	ret = pthread_create(&TCPSendThread, NULL, &TCPSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: TCPSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/******************************************************************************************************/
	/*	UDP Protocol - SEND
	UDPSendThread - call function UDPSendFunc(wait on sendUDPPipe, build UDP header and add it to message,
	write it to sendUDPPipe  */
	/*UDPSendThread*/
	ret = pthread_create(&UDPSendThread, NULL, &UDPSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: UDPSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	sleep(1);
	/*****************************************************************************************************
	/*	IP Protocol - SEND
	IPSendThread-call function IPSendFunc(wait on sendIPPipe, build the IP header and add it to message,
	write it to sendEthernetPipe	*/	
	/*IPSendThread*/
	ret = pthread_create(&IPSendThread, NULL, &IPSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: IPSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}	
	sleep(1);
	/*****************************************************************************************************/
	/* 	Ethernet Protocol - SEND
	EthernetSendThread-call EthernetSendFunc(continously wait on sendEthernetPipe,Add Header, use sendTo 
	and write to socket */
	/*EthernetSendThread*/
	ret = pthread_create(&EthernetSendThread, NULL, &EthernetSendFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error: EthernetSendThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}	
	sleep(1);

	/***********************************************************************************************************/
	/*	APPLICATION THREADS	     	   */	
	/***********************************************************************************************************/
	/*	FTP APPLICATION Thread
	AppFTPThread- Write 100 messages to sendFTPPipe */
	/*AppFTPThread*/
	
	ret = pthread_create(&AppFTPThread, NULL, &AppFTPFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:AppFTPThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	
	/***********************************************************************************************************/
	/*	Telnet APPLICATION Thread
	AppTelnetThread- Write 100 messages to sendTelnetPipe */
	/*AppTelnetThread*/
	ret = pthread_create(&AppTelnetThread, NULL, &AppTelnetFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:AppTelnetThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	
	/***********************************************************************************************************/
	/*	RDP APPLICATION Thread
	AppRDPThread- Write 100 messages to sendRDPPipe */
	/*AppRDPThread*/
	ret = pthread_create(&AppRDPThread, NULL, &AppRDPFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:AppRDPThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	
	/***********************************************************************************************************/
	/*	DNS Client APPLICATION Thread
	AppDNSCThread- Write 100 messages to sendDNSCPipe */
	/*AppFTPThread*/
	ret = pthread_create(&AppDNSCThread, NULL, &AppDNSCFunc, (void*)&m);
	if(ret != 0) 
	{
	        printf("Error:AppDNSCThread pthread_create() failed\n");
	        exit(EXIT_FAILURE);
	}
	
	/*************************************************************************************************/
	
	
	//to stop main thread from killing itself
	while(1)
	{
		if(count_ftp_send >=100 && count_rdp_send>=100 && count_telnet_send>=100 && count_dnsc_send>=100)
		{
				gettimeofday(&end,NULL);
	  			useconds = end.tv_usec - start.tv_usec;
				mtime = (useconds);
				cout << "time elapsed : " << mtime << " msecs " << endl;
				break;
		}
	}
}
