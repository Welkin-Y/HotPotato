#ifndef _NETWORK_FACTORY_H_
#define _NETWORK_FACTORY_H_

class NetworkFactory {
 public:
  int getServFd(const char* port);
  int getConnFd(int sockfd, char** ip);
  int getConnFd(const char* hostname, const char* port);
  int getPort(int sockfd);
};

#endif
