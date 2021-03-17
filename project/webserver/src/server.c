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
#include <pthread.h> //for threading , link with lpthread

#include <connection.h>

#define DIE(str) perror(str);exit(-1);
#define BUFSIZE 512
extern int optind, optopt; /* getopt */

void display_help()
{
  printf("\n-h Print help text.");
  printf("\n-p port Listen to port number port.");
  printf("\n-d Run as a daemon instead of as a normal program.");
  printf("\n-l log file Log to log file.");
  printf("\n-s [fork | thread]  Select request handling method.\n");
}


int main(int argc, char* argv[]) {

  int portnumber;
  int use_fork = 0;
  int use_thread = 0;
  struct sockaddr_in sin, client_address;
  int server_socket, client_socket;
  int addrlen;
  int next_char;
  struct protoent *protoent;
  int process_id;
  int port_received = 0;
  pthread_t thread_id;

  while ((next_char = getopt(argc, argv, ":h:H:p:s:d:")) != -1)
  {
    switch(next_char)
    {
      case 'h':
      case 'H':
        display_help();
        exit(0);
        break;
      case 'p':
        portnumber = atoi(optarg);
        port_received = 1;
        printf("\nPassed port number: %d \n", portnumber);
        break;
      case 's':
        if(strcmp("fork", optarg)== 0)
        {
          use_fork = 1;
        }
        else if(strcmp("thread", optarg)== 0)
        {
          use_thread = 1;
        }
        else if(strcmp("prefork", optarg)== 0)
        {
          DIE("Not implemented perfork option");
        }
        else
        {
          DIE("Unsupported option with flag -s, run with flag -h for Help")
        }
        break;
      case 'd':
        DIE("Not yet implemented Demon option");
        break;
      default:
        display_help();
        DIE("Not a Valid flag");
    }
  }

  // Validate if port not received during server start
  if(port_received == 0)
  {
    DIE("Limitation: Port number must be passed");
  }
  // Validate port number range
  else if( portnumber <= 1024)
  {
    DIE("Limitation: Reserved port number - use port number as > 1024");
  }

  protoent = getprotobyname("tcp"); 
        
  /* get a file descriptor for an IPv4 socket using TCP */
  if((server_socket = socket(AF_INET, SOCK_STREAM, protoent->p_proto)) == -1) {
    DIE("Socket Creation Failed.");
  }

  /* zero out the sockaddr_in struct */
  memset(&sin, 0, sizeof(sin));

  /* setup the struct to inform the operating system that we would like
   * to bind the socket the the given port number on any network
   * interface using IPv4 */
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(portnumber);

  /* perform bind call */
  if(bind(server_socket, (struct sockaddr*) &sin, sizeof(sin)) == -1) {
    DIE("Socket binding failed");
  }

  /* start listening for connections arriving on the bound socket 
   * here, we set the backlog of pending connections to 10*/
  if(listen(server_socket, 10) == -1) {
    DIE("Socket listen failed");
  }

  addrlen = sizeof(client_address);

  while(1)
  {
    /* wait for incoming connections;
     * the address information of the communication partner is placed in
     * the provided sockaddr_in struct */
    if ((client_socket = accept(server_socket, (struct sockaddr*) &client_address, (socklen_t*) &addrlen)) == -1) {
      DIE("Socket connection not accepted");
    }

    // Pack client connection details
    struct client_connection  client_conn;
    client_conn.client_address = client_address;
    client_conn.client_socket = client_socket;

    // Handle fork
    if (use_fork == 1)
    {
      process_id = fork();

      if(process_id < 0)
      {
        DIE("Error on Fork");
      }
      /* This is client process */
      if(process_id == 0)
      {
        close(server_socket);

        /* Handle client connection */
        handle_client_connection((void*)&client_conn);
      }
      /* This is server process */
      else
      {
        close(client_socket); 
      }
    }
    // handle as thread
    else if(use_thread == 1)
    {
      if( pthread_create(&thread_id , NULL ,  handle_client_connection , (void*) &client_conn) < 0)
      {
        DIE("Not able to create thread");
      }

      //Now join the thread, so that we don't terminate before the thread
      //pthread_join( thread_id , NULL);
    }
    else
    {
      /* Handle client connection */
      handle_client_connection((void*)&client_conn);
    }
  }
  /* close the file descriptors */
  close(client_socket);
  close(server_socket);
        
  exit(0);
}
