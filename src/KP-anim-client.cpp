/*****************************************************************************
File: KP-anim-client.cpp

Virtual Humans
Master in Computer Science
Christian Jacquemin, University Paris 11

Copyright (C) 2008 University Paris 11 
This file is provided without support, instruction, or implied
warranty of any kind.  University Paris 11 makes no guarantee of its
fitness for a particular purpose and is not liable under any
circumstances for any damages or loss whatsoever arising from the use
or inability to use this file or items derived from it.
******************************************************************************/
#include <GL/gl.h>           
#include <GL/glu.h>         
#include <GL/glut.h>    

#include <stdio.h>      
#include <stdlib.h>     
#include <string.h>     
#include <math.h>
#ifdef __APPLE__
    #include <limits.h>
#elif __linux
    #include <values.h>
#endif
#include <ctype.h>
#include <tiffio.h>     /* Sam Leffler's libtiff library. */

#include <fcntl.h>
#include <errno.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#else
//#define socklen_t int
#include <winsock2.h>
#include <Ws2tcpip.h>
//#include <wspiapi.h>
#endif

/// HOST ADDRESS 
class vc_MySocketAddresse {
public:
  bool d_valid;
  sockaddr_in d_sockAddresse;
  int port;
  char host[100];
  char host_IP[100];

  vc_MySocketAddresse() {
    memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
    *host = 0;
    *host_IP = 0;
    d_valid = false;
  }

  ~vc_MySocketAddresse(void) {
  }

  void IPAdd() {
    // gets the string that represents the address "a.b.c.d"
    char* s = inet_ntoa( d_sockAddresse.sin_addr );
    if( s == NULL )
      printf( "Erreur: IPAdd() ne peut convertir l'adresse.\n" );
    else
      strcpy( host_IP, s );
  }

  void InitFromResolver(char *_host, const int _port) {
    struct hostent 		*hostinfo;

    strcpy( host , _host );  
    port=_port;
    d_sockAddresse.sin_family=AF_INET;
    d_sockAddresse.sin_port=htons(port);

    hostinfo=gethostbyname(host);
    if (hostinfo==NULL) {
      fprintf( stdout , "Unknown host %s \n", _host);
      memset((char *)&d_sockAddresse,0,sizeof(d_sockAddresse));
      d_valid = false;
      return;
    }

    d_sockAddresse.sin_addr=*(struct in_addr*)hostinfo->h_addr;
    IPAdd();
    d_valid = true;
  } 

  void TestUDPConnection( void ) {
    vc_MySocketAddresse sock;
    char nam[100];

    gethostname(nam, 100);
    printf( "%s\n" , nam );
    sock.InitFromResolver(nam, 3120);

    char* s = inet_ntoa(sock.d_sockAddresse.sin_addr);

    printf( "%s\n" , s );
  }
};


//////////////////////////////////////////////////////////////////
// MAIN 
//////////////////////////////////////////////////////////////////

int main(int argc, char **argv) 
{
  // local server address
  unsigned int Local_server_port = 1979;

  // local server socket
  int SocketToLocalServer = -1;

  ///////////////////////////////
  // local server creation
  struct sockaddr_in localServAddr;

#ifndef _WIN32
  int SocketToLocalServerFlags;
  /* socket creation */
  SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, 0);

  if(SocketToLocalServer < 0) {
    printf( "Error: udp server cannot open socket to local server!\n" ); 
    throw(0);
  }
  else {
    // Read the socket's flags
    SocketToLocalServerFlags = fcntl(SocketToLocalServer, F_GETFL, 0);
    // Sets the socket's flags to non-blocking
    SocketToLocalServerFlags |= O_NONBLOCK;
    int ret = fcntl(SocketToLocalServer, F_SETFL, SocketToLocalServerFlags );
    if(ret < 0) {
      printf(  "Error: udp server cannot set flag to non-blocking: %s!" , 
	       strerror(errno) );
      throw(0);
    }
 }
#else
  // socket creation
  SocketToLocalServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(SocketToLocalServer < 0) {
    printf( "Error: udp server cannot open socket to local server!\n" ); 
    throw(0);
  }
  else {
    // Read the socket's flags
    unsigned long onoff=1;
    
    if (ioctlsocket(SocketToLocalServer, FIONBIO, &onoff) != 0){
      printf(  "Error: udp server cannot set flag to non-blocking: %s!" , 
	       strerror(errno) );
      throw(0);
    }
  }
#endif
    
  // bind local server port
  localServAddr.sin_family = AF_INET;
  localServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  localServAddr.sin_port = htons(Local_server_port);
  
  int rc = bind (SocketToLocalServer, 
		 (struct sockaddr *) &localServAddr,sizeof(localServAddr));
  if(rc < 0) {
    printf( "Error: cannot bind locat port number %d!" , Local_server_port );
    throw(0);
  }
  printf("udp server: waiting for data on port UDP %u\n", Local_server_port);

  while( true ) {

    // reads incoming UDP messages
    if( SocketToLocalServer >= 0 ) {
      char    message[1024];
      // init message buffer: Null values
      memset(message,0x0,1024);
    
      // receive message
      int n = recv(SocketToLocalServer, message, 1024, 0);
      if( n >= 0 ) {
	// scans the message and extract string & float values
	char KP_ID[256];
	int Keyframe;
	float KP_translation[3];
	// printf( "Message size %d\n" , n );
	// printf( "Rec.: [%s]\n" , message );
	sscanf( message , "%s %d %f %f %f" , KP_ID , &Keyframe ,
		KP_translation , KP_translation + 1 , KP_translation + 2 );
	printf( "ID [%s] KF [%d] tr (%.3f,%.3f,%.3f)\n" , KP_ID , Keyframe ,
		KP_translation[0] , KP_translation[1] , KP_translation[2] );
      }
    }
  }
}
