#include "ringserver.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

// Constructor
RingServer::RingServer(const char* port, int num_players, int num_hops)
    : num_players(num_players), num_hops(num_hops) {
  this->fd = this->networkFactory.getServFd(port);
  this->initGame();
}

// Destructor
RingServer::~RingServer() { this->closeGame(); }

// Connect to one player
void RingServer::connectOnePlayer(int id) {
  int port, fd;
  char* ip;
  PlayerInfo player;
  if ((fd = this->networkFactory.getConnFd(this->fd, &ip)) == -1) {
    fprintf(stderr, "Connect to player %d failed\n", id);
    exit(EXIT_FAILURE);
  }
  send(fd, &id, sizeof(id), 0);
  send(fd, &(this->num_players), sizeof(this->num_players), 0);
  recv(fd, &port, sizeof(port), 0);
  player.fd = fd;
  player.addr.port = port;
  memset(player.addr.ip, 0, MAX_IP_LEN);
  strncpy(player.addr.ip, ip, MAX_IP_LEN);
  free(ip);
  this->players.emplace_back(player);
}

// Connect to N players
void RingServer::connectPlayers() {
  for (int id = 0; id < this->num_players; id++) {
    this->connectOnePlayer(id);
    printf("Player %d is ready to play\n", id);
  }
}

// Send id player (id-1) IP:PORT
void RingServer::sendNeighborInfo(int id) {
  int neighbor_id = id > 0 ? id - 1 : this->num_players - 1;
  int fd = this->players[id].fd;
  send(fd, &(this->players[neighbor_id].addr), sizeof(addr_t), 0);
}

void RingServer::printNumPlayers() {
  printf("Players = %d\n", this->num_players);
}

void RingServer::printNumHops() { printf("Hops = %d\n", this->num_hops); }

void RingServer::printMaster() { printf("Potato Ringmaster\n"); }

// send potato to a player
void RingServer::sendPotato() {
  int randint = rand() % this->num_players;
  send((this->players)[randint].fd, &(this->potato), sizeof(this->potato), 0);
  printf("Ready to start the game, sending potato to player %d\n", randint);
}

// receive last potato
void RingServer::recvPotato() {
  fd_set readfds;
  int nfds = -1;
  for (int i = 0; i < this->num_players; i++) {
    nfds = std::max(nfds, (this->players)[i].fd);
  }
  FD_ZERO(&readfds);
  for (int i = 0; i < this->num_players; i++) {
    FD_SET((this->players)[i].fd, &readfds);
  }
  select(nfds + 1, &readfds, NULL, NULL, NULL);
  for (int i = 0; i < this->num_players; i++) {
    if (FD_ISSET((this->players)[i].fd, &readfds)) {
      recv((this->players)[i].fd, &(this->potato), sizeof(this->potato),
           MSG_WAITALL);
      break;
    }
  }
}

// init game
void RingServer::initGame() {
  this->printMaster();
  this->printNumPlayers();
  this->printNumHops();
  this->connectPlayers();
  for (int id = 0; id < this->num_players; id++) {
    this->sendNeighborInfo(id);
  }
  this->potato = {this->num_hops, 0, {0}};
  srand(time(NULL));
}

void RingServer::runGame() {
  if (potato.num_hops != 0) {
    this->sendPotato();
    this->recvPotato();
  }
}

void RingServer::printResult() {
  printf("Trace of potato:\n");
  for (int i = 0; i < this->potato.cnt; i++) {
    if (i == potato.cnt - 1)
      printf("%d\n", this->potato.path[i]);
    else
      printf("%d,", this->potato.path[i]);
  }
}

void RingServer::closeGame() {
  this->printResult();
  // send end signal to all players
  for (int i = 0; i < this->num_players; i++) {
    this->potato.num_hops = 0;
    send((this->players)[i].fd, &(this->potato), sizeof(this->potato), 0);
    close((this->players)[i].fd);
  }
  close(this->fd);
}