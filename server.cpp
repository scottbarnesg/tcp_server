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
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

/* Define Constants */
#define LISTEN_PORT "55555" // Port on which to listen for connections
#define CONNECT_BUFFER 10 // Number of pending connections to hold in buffer


int createSocket(struct addrinfo *server_info){
  int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  if (sockfd == -1){
    fprintf(stderr, "socket error: %s\n", gai_strerror(sockfd));
    exit(1);
  }
  return sockfd;
}

int bindAndListen(int sockfd, struct addrinfo *server_info){
  int res = bind(sockfd, server_info->ai_addr, server_info->ai_addrlen);
  if (res != 0){
    printf("Bind error - errno: %d\n", errno);
    exit(1);
  }
  res = listen(sockfd, CONNECT_BUFFER);
  if (res != 0){
    printf("Listen error - errno: %d\n", errno);
    exit(1);
  }
  printf("Listening for incoming connections on port %s\n", LISTEN_PORT);
  return res;
}

void *processManager(void *input){
  printf("Created process manager thread\n");
  pid_t done_pid;
  int pid_status;
  while(1){
    if (done_pid = waitpid(-1, &pid_status, 0) > 0){
      printf("Child processes terminated and has been successfully reaped\n");
    }
  }
}

int handleSession(int sessionfd, struct sockaddr_in remote_addr){
  char data[512];
  fd_set sess_fdset;
  struct timeval timeout;
  int fd_ready, byte_count;
  int first_packet = 1;
  FD_ZERO(&sess_fdset);
  while(1){
    FD_SET(sessionfd, &sess_fdset);
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    fd_ready = select(sessionfd+1, &sess_fdset, NULL, NULL, &timeout);
    if (fd_ready == -1){
      printf("Select error - errno: %d\n", errno);
      close(sessionfd);
      exit(1);
    }
    else if (fd_ready == 0){
      printf("Connection timed out\n");
      close(sessionfd);
      exit(1);
    }
    else if (FD_ISSET(sessionfd, &sess_fdset)){
      byte_count = read(sessionfd, data, sizeof(data));
      if (byte_count == 0){
        printf("Connection closed by peer\n");
        close(sessionfd);
        exit(0);
      }
      else{
        data[byte_count] = '\0';
        printf("Data: %s", data);
      }
    }
  }
}

int createSession(int sockfd, int sessionfd, struct sockaddr_in remote_addr){
  if (sessionfd == -1){
    printf("Session creation error\n");
    return 1;
  }
  pid_t new_fork = fork();
  if (new_fork == -1){
    printf("Failed to create process to handle connection from %s\n", inet_ntoa(remote_addr.sin_addr));
  }
  else if (new_fork == 0){
    close(sockfd);
    handleSession(sessionfd, remote_addr);
  }
  else{
    printf("Created process %d to serve %s\n", new_fork, inet_ntoa(remote_addr.sin_addr));
    close(sessionfd);
  }
}

int acceptConnections(int sockfd, struct sockaddr_in remote_addr, socklen_t addr_size){
  int sessionfd;
  while(1){
    sessionfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addr_size);
    printf("Accepted new connection from %s\n", inet_ntoa(remote_addr.sin_addr));
    createSession(sockfd, sessionfd, remote_addr);
  }
  close(sockfd);
}

int main(void){
  int status;
  struct addrinfo addr_info;
  struct addrinfo *server_info;
  memset(&addr_info, 0, sizeof addr_info);
  addr_info.ai_family = AF_INET;
  addr_info.ai_socktype = SOCK_STREAM;
  if ((status = getaddrinfo(NULL, LISTEN_PORT, &addr_info, &server_info)) != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  int sockfd = createSocket(server_info);
  status = bindAndListen(sockfd, server_info);
  struct sockaddr_in remote_addr;
  socklen_t addr_size;
  pthread_t proc_man_thread;
  int thread, *void_input;
  thread = pthread_create(&proc_man_thread, NULL, processManager, (void *)void_input);
  acceptConnections(sockfd, remote_addr, addr_size);
  (void) pthread_join(thread, NULL);
  freeaddrinfo(server_info);
  return 0;
}
