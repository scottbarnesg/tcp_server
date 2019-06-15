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
/* cpp libraries */
#include <string>

/* Define Constants */
#define CONNECT_BUFFER 10 // Number of pending connections to hold in buffer

// Global variables
int fork_counter = 0;
int connection_counter = 0;

class Socket {
  public:
    int sockfd;
    int status;
    std::string listen_port;
    struct addrinfo addr_info;
    struct addrinfo *server_info;
    int create ( void );
    int acceptConnections( void );
    Socket( int port );
    ~Socket( void );
  private:
    struct sockaddr_in remote_addr;
    socklen_t addr_size;
    int createSocket( void );
    int bindAndListen ( void );
    int handleSession ( int sessionfd );
    int createSession( int sessionfd );
};

Socket::Socket( int port ) {
  listen_port = std::to_string(port);
  memset(&addr_info, 0, sizeof addr_info);
  addr_info.ai_family = AF_INET;
  addr_info.ai_socktype = SOCK_STREAM;
}

Socket::~Socket(void) {
  freeaddrinfo(server_info);
}

int Socket::create(void) {
  if ((status = getaddrinfo(NULL, listen_port.c_str(), &addr_info, &server_info)) != 0){
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  sockfd = createSocket();
  status = bindAndListen();
}

int Socket::createSocket(void){
  sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
  if (sockfd == -1){
    fprintf(stderr, "socket error: %s\n", gai_strerror(sockfd));
    exit(1);
  }
  return sockfd;
}

int Socket::bindAndListen(void){
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
  printf("Listening for incoming connections on port %s\n", listen_port.c_str());
  return res;
}

int Socket::handleSession(int sessionfd){
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

int Socket::createSession(int sessionfd){
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
    handleSession(sessionfd);
  }
  else{
    printf("Created process %d to serve %s\n", new_fork, inet_ntoa(remote_addr.sin_addr));
    fork_counter++;
    printf("Connection count: %d\n", connection_counter);
    printf("Fork Count:       %d\n", fork_counter);
    close(sessionfd);
  }
}

int Socket::acceptConnections(void){
  int sessionfd;
  while(1){
    sessionfd = accept(sockfd, (struct sockaddr *)&remote_addr, &addr_size);
    connection_counter++;
    printf("Accepted new connection from %s\n", inet_ntoa(remote_addr.sin_addr));
    createSession(sessionfd);
  }
  close(sockfd);
}

void *processManager(void *input){
  printf("Created process manager thread\n");
  pid_t done_pid;
  int pid_status;
  while(1){
    if (done_pid = waitpid(-1, &pid_status, 0) > 0){
      printf("Child processes terminated and has been successfully reaped\n");
      fork_counter--;
      connection_counter--;
      if (fork_counter != connection_counter){
        printf("Error: thread = pthread_create(&proc_man_thread, NULL, processManager, (void *)void_input);Connection count does not equal fork count\n");
      }
    }
  }
}

int main(void){
  int port = 55555;
  Socket tcp_socket(port);
  tcp_socket.create();
  pthread_t proc_man_thread;
  int thread, *void_input;
  thread = pthread_create(&proc_man_thread, NULL, processManager, (void *)void_input);
  tcp_socket.acceptConnections();
  (void) pthread_join(thread, NULL);
  return 0;
}
