#ifndef _RING_MASTER_UTILS_H_
#define _RING_MASTER_UTILS_H_
#define MAX_IP_LEN 40
#include "potato.h"

struct addr_t {
  char ip[MAX_IP_LEN];
  int port;
} typedef addr_t;

struct player_info_t {
  int fd;
  addr_t addr;
} typedef PlayerInfo;

struct ring_master_t {
  int fd;
  int num_players;
  int num_hops;
  PlayerInfo** list;
} typedef RingMaster;

RingMaster* newMaster(const char* port, int num_players, int num_hops);
void destroyMaster(RingMaster*);
void initGame(RingMaster*);
void runGame(RingMaster*, Potato*);
void printResult(Potato*);
void closeGame(RingMaster*, Potato*);

void printMaster();
void printNumPlayers(RingMaster*);
void printNumHops(RingMaster*);
PlayerInfo* connectOnePlayer(int, int, int);
void connectPlayers(RingMaster*);
void destroyPlayers(PlayerInfo**, int);
void sendNeighbors(RingMaster*, int id);
void sendPotato(RingMaster*, Potato*);
void recvPotato(RingMaster*, Potato*);

#endif