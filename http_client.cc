#include "minet_socket.h"
#include <stdlib.h>
#include <ctype.h>

#define BUFSIZE 1024

int main(int argc,char *argv[])
{

  char *server_name;
  int server_port;
  char *server_path;
  char *req;
  bool ok;
  
  fd_set socks;

  /*parse args */
  if (argc != 5)
  {
    fprintf(stderr, "usage: http_client k|u server port path\n");
    exit(-1);
  }

  server_name = argv[2];
  server_port = atoi(argv[3]);
  server_path = argv[4];

  req = (char *)malloc(strlen("GET  HTTP/1.0\r\n\r\n")+
		       strlen(server_path)+1);  

  /* initialize */
  if (toupper(*(argv[1]))=='K') { 
    minet_init(MINET_KERNEL);
  } else if (toupper(*(argv[1]))=='U') { 
    minet_init(MINET_USER);
  } else {
    fprintf(stderr, "First argument must be k or u\n");
    exit(-1);
  }

  /* make socket */
  
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  /* get host IP address  */
  /* Hint: use gethostbyname() */
  
  struct hostent *host;
  
  host = gethostbyname(server_name);

  /* set address */
  
  struct sockaddr_in sin;
  
  memset(&sin, 0, sizeof(sin));
  
  sin.sin_family = AF_INET;
  sin.sin_port = htons(server_port);
  sin.sin_addr.s_addr = *(unsigned long *) host->h_addr_list[0];
  
  int sin_len = sizeof(sin);

  /* connect to the server socket */
  
  if(connect(sock, (struct sockaddr *) &sin, sizeof(sin)) != 0) {
  	close(sock);
  	return -1;
  }

  /* send request message */
  sprintf(req,"GET %s HTTP/1.0\r\n\r\n", server_path);

  /* wait till socket can be read. */
  
  FD_SET(sock, &socks);
  /* Hint: use select(), and ignore timeout for now. */
  int readsocks;
  
  readsocks = select(sock+1, &socks, (fd_set *) 0, (fd_set *) 0, NULL);

  /* first read loop -- read headers */
  
  if (readsocks > 0) {
  	FD_ISSET(sock, &socks);
  	int connection;
  	
  	connection = accept(sock, (struct sockaddr *) &sin, &sin_len);
  	if (connection < 0) {
  		perror("accept");
  		exit(EXIT_FAILURE);
  	}
  }
  
  char* buf;
  
  int rcount = 0;
  
  while (rcount != -1) {
  	rcount = recv(sock, &buf, BUFSIZE, 0);
  }
  /* examine return code */   

	//Skip "HTTP/1.0"
  	//remove the '\0'

	// Normal reply has return code 200

  /* print first part of response: header, error code, etc. */

  /* second read loop -- print out the rest of the response: real web content */

  /*close socket and deinitialize */
  
  close(sock);

  if (ok)
    return 0;
  else
    return -1;
}
