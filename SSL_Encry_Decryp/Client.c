/***************************************
*	Name - Tanvi Parikh
*	SID - 104321447
*	Programming Assignment 2
****************************************/

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
/**************************************************/
#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define RETURN_NULL(x) if ((x)==NULL) exit (1)
#define RETURN_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define RETURN_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(1); }
 
#define RSA_CLIENT_CERT "cacert.pem"
#define RSA_CLIENT_KEY "cakey.pem"
 
#define RSA_CLIENT_CA_CERT  "cacert.pem"
#define RSA_CLIENT_CA_PATH  "NULL"
 
#define ON      1
#define OFF     0
/***********************************************************/

#ifndef INADDR_NONE
#define INADDR_NONE     0xffffffff
#endif  /* INADDR_NONE */
static int verify_callback(int ok, X509_STORE_CTX *ctx);
extern int	errno;

int	errexit(const char *format, ...);

#define	LINELEN		128

/*------------------------------------------------------------------------
 * main - TCP client for ECHO service
 *------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
	char	*host = "localhost";	/* host to use if none supplied	*/
	char	*portnum = "5004";	/* default server port number	*/

	switch (argc) {
	case 1:
		host = "localhost";
		break;
	case 3:
		host = argv[2];
		/* FALL THROUGH */
	case 2:
		portnum = argv[1];
		break;
	default:
		fprintf(stderr, "usage: TCPecho [host [port]]\n");
		exit(1);
	}
	
	char buf[LINELEN+1];		/* buffer for one line of text	*/
	int	n;						/* socket descriptor, read count*/
	int	outchars, inchars;		/* characters sent and received	*/

	/*************************************************************/
	int     s;              /* socket descriptor                    */
	struct sockaddr_in sin; /* an Internet endpoint address         */
	/***************************************************************************/
	int err;
	char *str;
	int     verify_client = OFF; /* To verify a client certificate, set ON */
	SSL_CTX         *ctx;
	SSL            *ssl;
	SSL_METHOD      *meth;
	X509            *server_cert;
	EVP_PKEY        *pkey;
   /***************************************************************************/
	/* Load encryption & hashing algorithms for the SSL program */
	SSL_library_init();
	
	/* Load the error strings for SSL & CRYPTO APIs */
	SSL_load_error_strings();

	/* Create an SSL_METHOD structure (choose an SSL/TLS protocol version) */
	meth = TLSv1_method();

   /* Create an SSL_CTX structure */
	ctx = SSL_CTX_new(meth);                        

	RETURN_NULL(ctx);
	/****************************************************************************/
	if(verify_client == ON)
	{
		/* Load the client certificate into the SSL_CTX structure */
		if (SSL_CTX_use_certificate_file(ctx, RSA_CLIENT_CERT, SSL_FILETYPE_PEM) <= 0)
		{
			ERR_print_errors_fp(stderr);
			exit(1);
	 	}
     
		/* Load the private-key corresponding to the client certificate */
		if (SSL_CTX_use_PrivateKey_file(ctx, RSA_CLIENT_KEY, SSL_FILETYPE_PEM) <= 0)
		{
     		ERR_print_errors_fp(stderr);
			exit(1);
		}
		
		/* Check if the client certificate and private-key matches */
		if (!SSL_CTX_check_private_key(ctx))
		{
			fprintf(stderr,"Private key does not match the certificate public key\n");
			exit(1);
		}
	}
 
      /* Load the RSA CA certificate into the SSL_CTX structure */
      /* This will allow this client to verify the server's     */
      /* certificate.                                           */
	if (!SSL_CTX_load_verify_locations(ctx, RSA_CLIENT_CA_CERT, NULL))
	{
        ERR_print_errors_fp(stderr);
        exit(1);
    }
 
	/* Set flag in context to require peer (server) certificate */
	/* verification */

	SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,NULL);

	SSL_CTX_set_verify_depth(ctx,1);
   /* ------------------------------------------------------------- */
	struct hostent  *phe;   /* pointer to host information entry    */
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;

    /* Map port number (char string) to port number (int)*/
        if ((sin.sin_port=htons((unsigned short)atoi(portnum))) == 0)
                errexit("can't get \"%s\" port number\n", portnum);

    /* Map host name to IP address, allowing for dotted decimal */
        if ( phe = gethostbyname(host) )
                memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
        else if ( (sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE )
                errexit("can't get \"%s\" host entry\n", host);

    /* Allocate a socket */
        s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s < 0)
                errexit("can't create socket: %s\n", strerror(errno));

    /* Connect the socket */
        if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
                errexit("can't connect to %s.%s: %s\n", host, portnum,
                        strerror(errno));
       
	/*******************************************************************************************/	
	/* An SSL structure is created */
	sl = SSL_new (ctx);
    RETURN_NULL(ssl);
 
    /* Assign the socket into the SSL structure (SSL and socket without BIO) */
    SSL_set_fd(ssl, s);
 
	/* Perform SSL Handshake on the SSL client */
    err = SSL_connect(ssl);
    RETURN_SSL(err);
 
    /* Informational output (optional) */
    printf ("SSL connection using %s\n", SSL_get_cipher (ssl));
 
    /* Get the server's certificate (optional) */
    server_cert = SSL_get_peer_certificate (ssl);    
	if (server_cert != NULL)
    {
        printf ("Server certificate:\n");
		str = X509_NAME_oneline(X509_get_subject_name(server_cert),0,0);
        RETURN_NULL(str);
	    printf ("\t subject: %s\n", str);
	    free (str);
		str = X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0);
		RETURN_NULL(str);
	    printf ("\t issuer: %s\n", str);
		free(str);
		X509_free (server_cert);
    }
    else
                printf("The SSL server does not have certificate.\n");
	/*****************************************************************************************************/
 
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[LINELEN] = '\0';	/* insure line null-terminated	*/
		outchars = strlen(buf);
		(void) SSL_write(ssl, buf, outchars);

		/* read it back */
		for (inchars = 0; inchars < outchars; inchars+=n ) {
			n = SSL_read(ssl, &buf[inchars], outchars - inchars);
			if (n < 0)
				errexit("socket read failed: %s\n",
					strerror(errno));
		}
		fputs(buf, stdout);
		/*--------------- SSL closure ---------------*/
        /* Shutdown the client side of the SSL connection */
 
        //err = SSL_shutdown(ssl);
        //RETURN_SSL(err);
 
        /* Terminate communication on a socket */
        //err = close(s);
 
        //RETURN_ERR(err, "close");
 
        /* Free the SSL structure */
        //SSL_free(ssl);
 
        /* Free the SSL_CTX structure */
        //SSL_CTX_free(ctx);
	}
/*}*/
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

/*------------------------------------------------------------------------
 * connectsock - allocate & connect a socket using TCP 
 *------------------------------------------------------------------------
 */
/*int
connectsock(const char *host, const char *portnum)
/*
 * Arguments:
 *      host      - name of host to which connection is desired
 *      portnum   - server port number
 */
//{
        
//}*/


