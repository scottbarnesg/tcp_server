#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define PORT 55555
#define ADDRESS "127.0.0.1"

int createSocket(void){
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1){
    fprintf(stderr, "socket error: %s\n", gai_strerror(sockfd));
    exit(1);
  }
  return sockfd;
}

int connectToServer(int sockfd, struct sockaddr_in server_address){
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDRESS, &server_address.sin_addr)<=0){
    printf("Invalid server IP address provided - exiting\n");
    return -1;
  }
  int status = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
  if (status < 0){
    printf("Failed to connect to server - exiting\n");
    printf("Errno: %d\n", errno);
    return -1;
  }
  return 0;
}

int talk2Server(int sockfd, char *message){
  while(1){
    send(sockfd, message, strlen(message), 0);
    sleep(1);
  }
}

int main(void){
  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  int sockfd = createSocket();
  if (connectToServer(sockfd, server_address) < 0){
    exit(1);
  }
  char message[1024] = "Hello world\n";
  talk2Server(sockfd, message);
}
