#ifndef _PLAYER_UTILS_H_
#define _PLAYER_UTILS_H_
#define MAX_IP_LEN 40
#include "potato.h"

struct addr_t {
  char ip[MAX_IP_LEN];
  int port;
} typedef addr_t;

struct ring_player_t {
  int num_players;
  int id;
  int fd;
  int master_fd;
  int left_fd;
  int right_fd;
} typedef RingPlayer;

RingPlayer* newPlayer();
void destroyPlayer(RingPlayer*);
void connectMaster(RingPlayer*, const char* hostname, const char* port);
void connectPeers(RingPlayer*);
void playGame(RingPlayer*);
void closeGame(RingPlayer*);
void sendToPeers(RingPlayer*, Potato*);

#endif