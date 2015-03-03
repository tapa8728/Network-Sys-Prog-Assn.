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

#define	QLEN 32		// maximum connection queue length
#define	BUFSIZE	4096
/*********************************************************/
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
 
#define RSA_SERVER_CERT "server.cert"
#define RSA_SERVER_KEY  "server_priv.key"
 
#define RSA_SERVER_CA_CERT "server.cert"
#define RSA_SERVER_CA_PATH  "NULL"
 
#define ON   1
#define OFF  0
 
#define RETURN_NULL(x) if ((x)==NULL) exit(1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }
/**********************************************************/

extern int	errno;
int	errexit(const char *format, ...);

/*------------------------------------------------------------------------
 * main - Concurrent TCP server for ECHO service
 *------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
	char *portnum = "5004";		/* Standard server port number	*/
	struct sockaddr_in fsin;	/* the from address of a client	*/
	int	msock;					/* master server socket		*/
	fd_set	rfds;				/* read file descriptor set	*/
	fd_set	afds;				/* active file descriptor set	*/
	unsigned int alen;			/* from-address length		*/
	int	fd, nfds;
	SSL *ssl_array[100];
	/*******************/
	char *str;
	int err;
	int verify_client = OFF; 
	/*********************/
	SSL_CTX         *ctx;
    SSL_METHOD      *meth;
    X509            *client_cert = NULL;
	char buf[BUFSIZ];
	int	cc;
	int i,j,k,m;
	/*******************/
	
	switch (argc)
	{
		case	1:
			break;
		case	2:
			portnum = argv[1];
			break;
		default:
			errexit("usage: TCPmechod [port]\n");
	}
	
    /**********************************************************/
	/* Load encryption & hashing algorithms for the SSL program */
	SSL_library_init();
 
    /* Load the error strings for SSL & CRYPTO APIs */
    SSL_load_error_strings();
 
    /* Create a SSL_METHOD structure (choose a SSL/TLS protocol version) */
    meth = TLSv1_method();
 
    /* Create a SSL_CTX structure */
    ctx = SSL_CTX_new(meth);
    if (!ctx)
	{
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* Load the server certificate into the SSL_CTX structure */
    if (SSL_CTX_use_certificate_file(ctx, RSA_SERVER_CERT, SSL_FILETYPE_PEM) <= 0)
	{
        ERR_print_errors_fp(stderr);
		exit(1);
    }
 
    /* Load the private-key corresponding to the server certificate */
    if (SSL_CTX_use_PrivateKey_file(ctx, RSA_SERVER_KEY, SSL_FILETYPE_PEM) <= 0)
	{
		ERR_print_errors_fp(stderr);
        exit(1);
    }
 
    /* Check if the server certificate and private-key matches */
    if (!SSL_CTX_check_private_key(ctx))
	{
        fprintf(stderr,"Private key does not match the certificate public key\n");
        exit(1);
    }
 
	//to do client-side verification
    if(verify_client == ON)
    {
        /* Load the RSA CA certificate into the SSL_CTX structure */
        if (!SSL_CTX_load_verify_locations(ctx, RSA_SERVER_CA_CERT, NULL))
		{
            ERR_print_errors_fp(stderr);
            exit(1);
        }
        /* Set to require peer (client) certificate verification */
        SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);
        /* Set the verification depth to 1 */
        SSL_CTX_set_verify_depth(ctx,1);
    }
	
	/*********************************************************/
	//msock = passivesock(portnum, QLEN);
	struct sockaddr_in sin; /* an Internet endpoint address  */
    int s;              /* socket descriptor             */
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    /* Map port number (char string) to port number (int) */
    if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
         errexit("can't get \"%s\" port number\n", portnum);

    /* Allocate a socket */
	s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s < 0)
            errexit("can't create socket: %s\n", strerror(errno));

    /* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		fprintf(stderr, "can't bind to %s port: %s; Trying other port\n", portnum, strerror(errno));
		sin.sin_port=htons(0); /* request a port number to be allocated by bind */
		if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
			errexit("can't bind: %s\n", strerror(errno));
		else
		{
			int socklen = sizeof(sin);
			if (getsockname(s, (struct sockaddr *)&sin, &socklen) < 0)
					errexit("getsockname: %s\n", strerror(errno));
			printf("New server port number is %d\n", ntohs(sin.sin_port));
		}
	}

	if (listen(s, 5) < 0)
		errexit("can't listen on %s port: %s\n", portnum, strerror(errno));
	/********************************************************/
	/* TCP connection is ready. */
   
	nfds = getdtablesize();
	FD_ZERO(&afds);
	FD_SET(s, &afds);

	while (1)
	{
		memcpy(&rfds, &afds, sizeof(rfds));
		if (select(nfds, &rfds, (fd_set *)0, (fd_set *)0, (struct timeval *)0) < 0)
			errexit("select: %s\n", strerror(errno));
			
		if (FD_ISSET(s, &rfds))
		{
			int	ssock;
			alen = sizeof(fsin);
			ssock = accept(s, (struct sockaddr *)&fsin, &alen);
			if (ssock < 0)
				errexit("accept: %s\n",	strerror(errno));
			FD_SET(ssock, &afds);
			
			/* A SSL structure is created */
			ssl_array[ssock] = SSL_new(ctx);
			SSL_set_fd(ssl_array[ssock], ssock);
			if( SSL_accept(ssl_array[ssock]) < 0)
			{
				ERR_print_errors_fp(stderr);
			}			   
		}
 		
		for (fd=0; fd<nfds; ++fd)
		{	
			if (fd != s && FD_ISSET(fd, &rfds))
			{
				cc = SSL_read(ssl_array[fd], buf, sizeof buf);
				if (cc < 0)
					errexit("echo read: %s\n", strerror(errno));
				if (cc && SSL_write(ssl_array[fd], buf, cc) < 0)
					errexit("echo write: %s\n", strerror(errno));
				if(cc == 0)
				{
					(void) close(fd);
					FD_CLR(fd, &afds);
				}
			}
		} 
	}
}

/*------------------------------------------------------------------------
 * errexit - print an error message and exit
 *------------------------------------------------------------------------
 */
int
errexit(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	exit(1);
}
