#include "networkfactory.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
const int ERROR = -1;

// as server listening tcp, return sockfd
inline int NetworkFactory::getServFd(const char* port) {
  struct addrinfo hints, *res;
  int status;
  int sockfd;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  // getaddrinfo()
  if ((status = getaddrinfo(NULL, port, &hints, &res)) == ERROR) {
    fprintf(stderr, "getaddrinfo error\n");
    exit(EXIT_FAILURE);
  }

  // socket()
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) ==
      ERROR) {
    fprintf(stderr, "socket fd error\n");
    exit(EXIT_FAILURE);
  }

  int reuse = 1;
  if ((status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                           sizeof(reuse))) == ERROR) {
    fprintf(stderr, "setsockopt error\n");
    exit(EXIT_FAILURE);
  }

  // bind()
  if ((status = bind(sockfd, res->ai_addr, res->ai_addrlen)) == ERROR) {
    fprintf(stderr, "bind error\n");
    exit(EXIT_FAILURE);
  }

  // listen()
  if ((status = listen(sockfd, 20)) == -1) {
    fprintf(stderr, "listen error\n");
    exit(EXIT_FAILURE);
  }
  freeaddrinfo(res);
  return sockfd;
}

// as server accepting tcp connetion, return connfd
int NetworkFactory::getConnFd(int sockfd, char** ip) {
  int connfd;
  struct sockaddr_storage sock_addr;
  socklen_t sock_addr_len = sizeof(sock_addr);
  if ((connfd = accept(sockfd, (struct sockaddr*)&sock_addr, &sock_addr_len)) ==
      ERROR) {
    fprintf(stderr, "accept error\n");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in* addr = (struct sockaddr_in*)&sock_addr;
  if (ip != NULL) *ip = strdup(inet_ntoa(addr->sin_addr));
  return connfd;
}

// connect to hostname:port, return sockfd
int NetworkFactory::getConnFd(const char* hostname, const char* port) {
  int status;
  int sockfd;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  // getaddrinfo()
  if ((status = getaddrinfo(hostname, port, &hints, &res)) == ERROR) {
    fprintf(stderr, "failed getaddrinfo to %s:%s\n", hostname, port);
    exit(EXIT_FAILURE);
  }

  // socket()
  if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) ==
      ERROR) {
    fprintf(stderr, "socket error\n");
    exit(EXIT_FAILURE);
  }

  // connect()
  if ((status = connect(sockfd, res->ai_addr, res->ai_addrlen)) == ERROR) {
    fprintf(stderr, "connect error\n");
    exit(EXIT_FAILURE);
  }

  freeaddrinfo(res);
  return sockfd;
}

// get port by sockfd
int NetworkFactory::getPort(int sockfd) {
  struct sockaddr_in sin;
  socklen_t len = sizeof(sin);
  if (getsockname(sockfd, (struct sockaddr*)&sin, &len) == ERROR) {
    fprintf(stderr, "getsockname error\n");
    exit(EXIT_FAILURE);
  }
  return ntohs(sin.sin_port);
}
