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
#define LISTEN_PORT "3490" // Port on which to listen for connections
#define CONNECT_BUFFER 10 // Number of pending connections to hold in buffer

/* Get server address information */
struct addrinfo getServerInfo(struct addrinfo addr_info, struct addrinfo *server_info){
  int status;
  // struct addrinfo addr_info; //, *server_info;

  memset(&addr_info, 0, sizeof addr_info); // Zero memory in struct

  addr_info.ai_family = AF_UNSPEC; // IP protocol agnostic
  addr_info.ai_socktype = SOCK_STREAM; // TCP stream socket
  addr_info.ai_flags = AI_PASSIVE; // Get address info from host

  status = getaddrinfo(NULL, LISTEN_PORT, &addr_info, &server_info);
  if(status != 0){
    perror("getaddrinfo error: unable to get address info on declared port");
  }
  return addr_info;
}

/* Make, bind, and listen to socket */
int setupSocket(struct addrinfo *server_info){
  int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol); // Error
  printf("Created Socket\n");
  bind(sockfd, server_info->ai_addr, server_info->ai_addrlen);
  printf("Bound to Socket\n");
  listen(sockfd, CONNECT_BUFFER);
  return sockfd;
}

int main(void){
  printf("Starting\n");
  struct addrinfo addr_info;
  struct addrinfo *server_info = malloc(sizeof(*server_info));
  struct sockaddr_storage client_addr;
  socklen_t addr_size;
  printf("Declared Variables\n");
  addr_info = getServerInfo(addr_info, server_info);
  printf("Got server info\n");
  int sockfd = setupSocket(server_info);

  shutdown(sockfd, 2);
  free(server_info);
}
