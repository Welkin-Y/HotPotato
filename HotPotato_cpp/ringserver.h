#ifndef _RING_SERVER_H_
#define _RING_SERVER_H_
#include <vector>

#include "networkfactory.cpp"
#include "playerinfo.h"
#include "potato.h"

class RingServer {
 public:
  RingServer() = default;
  RingServer(const char* port, int num_players, int num_hops);
  ~RingServer();
  void runGame();

 public:
  Potato potato;

 private:
  void initGame();

  void printMaster();
  void printNumPlayers();
  void printNumHops();

  void connectOnePlayer(int id);
  void connectPlayers();
  void sendNeighborInfo(int id);

  void sendPotato();
  void recvPotato();

  void printResult();
  void closeGame();

 private:
  int fd;
  int num_players;
  int num_hops;
  std::vector<PlayerInfo> players;
  NetworkFactory networkFactory;
};

#endif