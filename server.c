/* Network Libraries */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
/* Standard Libraries */
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/* Define Constants */
#define LISTEN_PORT "60" // Port on which to listen for connections
#define CONNECT_BUFFER 10 // Number of pending connections to hold in buffer


int createSocket(struct addrinfo *server_info){
  int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  if (sockfd == -1){
    fprintf(stderr, "socket error: %s\n", gai_strerror(sockfd));
    exit(1);
  }
  printf("Successfully created socket\n");
  return sockfd;
}

int main(void){
  int status;
  struct addrinfo addr_info;
  struct addrinfo *server_info;
  memset(&addr_info, 0, sizeof addr_info);
  addr_info.ai_family = AF_INET;
  addr_info.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(NULL, "60", &addr_info, &server_info)) != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  else{
    printf("Successfully got address info\n");
  }
  int sockfd = createSocket(server_info);
  status = bind(sockfd, server_info->ai_addr, server_info->ai_addrlen);
  printf("%d\n", status);
  status = listen(sockfd, CONNECT_BUFFER);
  printf("%d\n", status);
  struct sockaddr_storage remote_addr;
  socklen_t addr_size;
  status = accept(sockfd, (struct sockaddr *)&remote_addr, &addr_size);
  printf("After accept\n");
  freeaddrinfo(server_info);
}
