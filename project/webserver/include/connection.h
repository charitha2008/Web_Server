#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>

struct client_connection
{
  int client_socket;
  struct sockaddr_in client_address;
};

//the thread function - declare function pointer type
void *handle_client_connection(void *);

//void handle_client_connection(void* client_conn_p);
