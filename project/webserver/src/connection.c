#include <connection.h>
#include <response.h>

#define BUFSIZE 512
char index_file[] ="../www/index.html";

void *handle_client_connection(void* client_conn_p)
{
  char ipAddress[INET_ADDRSTRLEN];
  char buf[BUFSIZE];
  char *first_line;
  int file_desc;
  char req_type[8];
  char req_path[1024];
  char req_prot[128];
  int rec_bytes;
  int client_socket;
  struct sockaddr_in *client_address_p;
  struct sockaddr_in client_address;
  struct client_connection client_conn;

  // type cast to client_connection and extract client socket to read request details
  client_conn = *(struct client_connection*)client_conn_p;
  client_address = client_conn.client_address;
  client_socket = client_conn.client_socket;

  // get reference to client address
  client_address_p = &client_address;

  /* receive at most sizeof(buf)-1 many bytes and store them in the buffer */
  if ((rec_bytes = recv(client_socket, buf, sizeof(buf)-1, 0)) == -1) {
    // if number bytes less than zero - it means error
    perror("Receive data from socket Error..");exit(-1);
  }

  // Terminate request with NULL
  buf[rec_bytes] = '\0';

  /* convert IP address of communication partner to string */
  inet_ntop(AF_INET, &(client_address_p->sin_addr), ipAddress, sizeof(ipAddress));
	
  /* print out client information and received message */
  printf("\nRequest from %s:%i\n", ipAddress, ntohs(client_address_p->sin_port));
  printf("\nMessage: %s\n",buf);

  // get first line of request
  first_line = buf;
  
  // Header is in first line and parse for request parameters
  sscanf(first_line, "%s %s %s",req_type, req_path, req_prot);

  printf("Request type: %s \n", req_type);
  printf("Request path: %s \n", req_path);
  printf("Request protocol: %s \n", req_prot);

  // when request type is HEAD - must bnot contain body in response
  if(strcmp(req_type,"HEAD") == 0)
  {
    send_response(client_socket, "HTTP/1.1 200 OK", "text/html", NULL);
  }
  else if (strcmp(req_type, "GET") == 0)
  {
    // check for root request
    if ( strcmp(req_path, "/") == 0)
    {
      char *response_body = "<html><head>WebServer Demo..</head><body><h1>You have reached server root!</h1></body></html>";
      send_response(client_socket, "HTTP/1.1 200 OK", "text/html", response_body);
    }
    else if(strcmp(req_path, "/index.html") == 0)
    {
      //char *response_body = "<html><head></head><body><h1>You have reached Server..!</h1></body></html>";
      //send_response(client_socket, "HTTP/1.1 200 OK", "text/html", response_body);
      file_desc = open(index_file,O_RDONLY);

      if(sendfile(client_socket, file_desc, NULL, 1000) == -1)
      {
        perror("Send file is failed");exit(-1);
      }
      /* Close file descriptor */
      close(file_desc);
    }
    else
    {
      char resp_body[1024];
      sprintf(resp_body, "404: %s Not Found", req_path);
      send_response(client_socket, "HTTP/1.1 404 NOT FOUND", "text/html", resp_body);
    }
  }
  else
  {
    char resp_body[1024];
    sprintf(resp_body, "501: %s not found", req_path);
    send_response(client_socket, "HTTP/1.1 501 NOT IMPLEMENTED", "text/html", resp_body);
  }

  close(client_socket);

  return 0;
}
