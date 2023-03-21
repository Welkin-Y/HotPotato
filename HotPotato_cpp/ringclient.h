#ifndef _RING_CLIENT_H_
#define _RING_CLIENT_H_

#include "addr.h"
#include "networkfactory.cpp"
#include "potato.h"

class RingClient {
 public:
  RingClient() = default;
  RingClient(const char* hostname, const char* port);
  ~RingClient();

  void playGame(Potato&);

 private:
  void connectMaster(const char* hostname, const char* port);
  void connectPeers();
  void connLeftPeer();
  void connRightPeer();
  void sendToPeers(Potato&);
  void closeGame();

 private:
  int num_players;
  int id;
  int fd;

  int master_fd;
  int right_fd;
  int left_fd;
  addr_t addr;

  NetworkFactory networkFactory;
};

#endif