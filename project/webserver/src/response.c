#include <response.h>

int send_response(int fd, char *header, char *cont_type, char *body)
{
  char resp[65536]; 

  // save current time in http header
  time_t t1 = time(NULL); 
  struct tm *ltime = localtime(&t1);

  // get byte size for body
  int length = strlen(body); 
  printf("Size of body: %d\n", length);
  int resp_len = sprintf(resp, 
      "%s\n" 
      "Content-Length: %d\n" 
      "Content-Type: %s\n" 
      "Date: %s" // asctime adds its own newline 
      "Connection: close\n" 
      "\n" // End of HTTP header 
      "%s", 
      header, 
      length, 
      cont_type, 
      asctime(ltime), 
      body); 
  int resp_code = send(fd, resp, resp_len, 0); 
  if (resp_code < 0) { 
      perror("send"); 
  } 
  return resp_code;
}
