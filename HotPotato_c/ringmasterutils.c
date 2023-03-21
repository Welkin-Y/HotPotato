#include "ringmasterutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "utils.c"

extern int getConnFd(int master_fd, char** ip);
extern int getServFd(const char* port);
extern void memErr();

// Connect to one player
inline PlayerInfo* connectOnePlayer(int num_players, int player_id,
                                    int master_fd) {
  int port, fd;
  char* ip;
  PlayerInfo* player;
  if ((fd = getConnFd(master_fd, &ip)) == -1) {
    fprintf(stderr, "Connect to player %d failed\n", player_id);
    exit(EXIT_FAILURE);
  }
  if ((player = malloc(sizeof(PlayerInfo))) == NULL) memErr();
  send(fd, &player_id, sizeof(player_id), 0);
  send(fd, &num_players, sizeof(num_players), 0);
  recv(fd, &port, sizeof(port), 0);
  player->fd = fd;
  player->addr.port = port;
  memset(player->addr.ip, 0, MAX_IP_LEN);
  strncpy(player->addr.ip, ip, MAX_IP_LEN);
  free(ip);
  return player;
}

// Connect to N players
inline void connectPlayers(RingMaster* self) {
  if ((self->list = malloc(self->num_players * sizeof(PlayerInfo*))) == NULL)
    memErr();
  for (int i = 0; i < self->num_players; i++) {
    (self->list)[i] = connectOnePlayer(self->num_players, i, self->fd);
    printf("Player %d is ready to play\n", i);
  }
}

// Send i player (i-1) IP:PORT
inline void sendNeighbors(RingMaster* self, int i) {
  int id = i > 0 ? i - 1 : self->num_players - 1;
  int fd = (self->list)[i]->fd;
  send(fd, &((self->list)[id]->addr), sizeof(addr_t), 0);
}

// Free player list
inline void destroyPlayers(PlayerInfo** player_list, int num_players) {
  for (int i = 0; i < num_players; i++) {
    free(player_list[i]);
  }
  free(player_list);
}

// Create ring master
inline RingMaster* newMaster(const char* port, int num_players, int num_hops) {
  RingMaster* master;
  if ((master = malloc(sizeof(RingMaster))) == NULL) memErr();
  master->num_players = num_players;
  master->num_hops = num_hops;
  master->fd = getServFd(port);
  return master;
}

// Free ring master
inline void destroyMaster(RingMaster* self) {
  destroyPlayers(self->list, self->num_players);
  free(self);
}

inline void printNumPlayers(RingMaster* self) {
  printf("Players = %d\n", self->num_players);
}

inline void printNumHops(RingMaster* self) {
  printf("Hops = %d\n", self->num_hops);
}

inline void printMaster() { printf("Potato Ringmaster\n"); }

// send potato to a player
inline void sendPotato(RingMaster* self, Potato* potato) {
  srand(time(NULL));
  int randint = rand() % self->num_players;
  send((self->list)[randint]->fd, potato, sizeof(*potato), 0);
  printf("Ready to start the game, sending potato to player %d\n", randint);
}

// receive last potato
inline void recvPotato(RingMaster* self, Potato* potato) {
  fd_set readfds;
  int nfds = -1;
  for (int i = 0; i < self->num_players; i++) {
    nfds = MAX(nfds, (self->list)[i]->fd);
  }
  FD_ZERO(&readfds);
  for (int i = 0; i < self->num_players; i++) {
    FD_SET((self->list)[i]->fd, &readfds);
  }
  select(nfds + 1, &readfds, NULL, NULL, NULL);
  for (int i = 0; i < self->num_players; i++) {
    if (FD_ISSET((self->list)[i]->fd, &readfds)) {
      recv((self->list)[i]->fd, potato, sizeof(*potato), MSG_WAITALL);
      break;
    }
  }
}

// init game
void initGame(RingMaster* self) {
  printMaster();
  printNumPlayers(self);
  printNumHops(self);
  connectPlayers(self);
  for (int i = 0; i < self->num_players; i++) {
    sendNeighbors(self, i);
  }
}

void runGame(RingMaster* self, Potato* potato) {
  if (potato->num_hops != 0) {
    sendPotato(self, potato);
    recvPotato(self, potato);
  }
}

void printResult(Potato* potato) {
  printf("Trace of potato:\n");
  for (int i = 0; i < potato->cnt; i++) {
    if (i == potato->cnt - 1)
      printf("%d\n", potato->path[i]);
    else
      printf("%d,", potato->path[i]);
  }
}

void closeGame(RingMaster* self, Potato* potato) {
  // send end signal to all players
  for (int i = 0; i < self->num_players; i++) {
    potato->num_hops = 0;
    send((self->list)[i]->fd, potato, sizeof(*potato), 0);
    close((self->list)[i]->fd);
  }
  close(self->fd);
  destroyMaster(self);
}