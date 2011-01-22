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
  
  int sock = minet_socket(SOCK_STREAM);
  
  if (sock < 0) {
  	cerr << "Error creating socket.";
  	minet_perror("Reason: ");
  	minet_close(sock);
  	return 0;
  }

  /* get host IP address  */
  /* Hint: use gethostbyname() */
  
  struct hostent *host;
  
  host = gethostbyname(server_name);

  /* set address */
  
  struct sockaddr_in serv, client;
  
  memset(&server, 0, sizeof(serv));
  
  serv.sin_family = AF_INET;
  serv.sin_port = htons(server_port);
  serv.sin_addr.s_addr = *(unsigned long *) host->h_addr_list[0];
  
  int serv_len = sizeof(serv);
  
  memset(&client, 0, sizeof(client));
  
  client.sin_family = AF_INET;
  client.sin_port = htons(0);
  client.sin_addr.s_addr = htonl(INADDR_ANY);
  
  int client_len = sizeof(client);

  /* connect to the server socket */
  
  if(minet_bind(sock, &client) < 0) {
  	minet_close(sock);
  	minet_deinit();
  	return 0;
  }
  
  if (minet_connect(sock, &serv) < 0) {
  	minet_close(sock);
  	minet_deinit();
  	return 0;
  }

  /* send request message */
  sprintf(req,"GET %s HTTP/1.0\r\n\r\n", server_path);

  /* wait till socket can be read. */
  
  FD_SET(sock, &socks);
  /* Hint: use select(), and ignore timeout for now. */
  int writesocks, readsocks;
  
  if (writesocks = minet_write(sock, req, strlen(req)) < 0) {
  	minet_close(sock);
  	minet_deinit();
  	return 0;
  }
  
  FD_ZERO(&readsocks);
  FD_SET(sock, &readsocks);
  
  maxfds = sock + 1;
  select(maxfds, &readsocks, NULL, NULL, NULL);
  
  int readclient;
  
  if (FD_ISSET(sock, &readsocks)) {
  	while (1) {
  		if(readclient=minet_read(sock, incoming, 80) < 0) {
  			minet_close(sock);
  			minet_deinit();
  			return 0;
  		}
  		
  		if(readclient == 0) {
  			break;
  		}
  		
  		cout << incoming << endl;
  	}
  }
  
  // while loop
  // accept
  // fork
  // etc.

  /* first read loop -- read headers */
 
  /* examine return code */   

	//Skip "HTTP/1.0"
  	//remove the '\0'

	// Normal reply has return code 200

  /* print first part of response: header, error code, etc. */

  /* second read loop -- print out the rest of the response: real web content */

  /*close socket and deinitialize */
  
  minet_close(sock);
  minet_deinit();

  if (ok)
    return 0;
  else
    return -1;
}
