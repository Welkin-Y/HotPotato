#include "ringclient.h"

#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <string>

using std::string;
using std::to_string;

// Connect to Ring master
RingClient::RingClient(const char* hostname, const char* port) {
  this->connectMaster(hostname, port);
  this->connectPeers();
}

RingClient::~RingClient() { this->closeGame(); }

void RingClient::connectMaster(const char* hostname, const char* port) {
  this->master_fd = this->networkFactory.getConnFd(hostname, port);
  recv(this->master_fd, &(this->id), sizeof(this->id), 0);
  recv(this->master_fd, &(this->num_players), sizeof(this->num_players), 0);
  this->fd = this->networkFactory.getServFd("");
  int player_port = this->networkFactory.getPort(this->fd);
  send(this->master_fd, &player_port, sizeof(player_port), 0);
  printf("Connected as player %d out of %d total players\n", this->id,
         this->num_players);
}

// Connect left neighbor
void RingClient::connLeftPeer() {
  recv(this->master_fd, &(this->addr), sizeof(this->addr),
       MSG_WAITALL);  // recv (id-1)th IP:PORT
  this->left_fd = this->networkFactory.getConnFd(
      this->addr.ip, to_string(this->addr.port).c_str());
}

// Connect right neighbor
void RingClient::connRightPeer() {
  this->right_fd = this->networkFactory.getConnFd(
      this->fd, NULL);  // accept (id+1) connection
}

// Connect neighbors
void RingClient::connectPeers() {
  this->connLeftPeer();
  this->connRightPeer();
}

// Send potato to neighbors
void RingClient::sendToPeers(Potato& potato) {
  int rand_int = rand() & 1;
  if (rand_int == 0) {
    send(this->left_fd, &potato, sizeof(potato), 0);
    int left_id = this->id == 0 ? this->num_players - 1 : this->id - 1;
    printf("Sending potato to %d\n", left_id);
  } else {
    send(this->right_fd, &potato, sizeof(potato), 0);
    int right_id = this->id + 1 == this->num_players ? 0 : this->id + 1;
    printf("Sending potato to %d\n", right_id);
  }
}

// Run game
void RingClient::playGame(Potato& potato) {
  fd_set readfds;
  int fds[3] = {this->left_fd, this->right_fd, this->master_fd};
  int nfds = std::max(std::max(this->left_fd, this->right_fd), this->master_fd);
  srand(time(NULL));

  // while true
  while (1) {
    // select
    FD_ZERO(&readfds);
    for (int i = 0; i < 3; i++) {
      FD_SET(fds[i], &readfds);
    }
    select(nfds + 1, &readfds, NULL, NULL, NULL);
    int data_len;
    for (int i = 0; i < 3; i++) {
      if (FD_ISSET(fds[i], &readfds)) {
        data_len = recv(fds[i], &potato, sizeof(potato), MSG_WAITALL);
        break;
      }
    }

    if (potato.num_hops != 0 && data_len != 0) {  // if not to end game
      potato.num_hops--;
      potato.path[potato.cnt] = this->id;
      potato.cnt++;
      // if num hops == 0
      if (potato.num_hops != 0) {
        sendToPeers(potato);
      } else {  // if I am it
        send(this->master_fd, &potato, sizeof(potato), 0);
        printf("I'm it\n");
        return;
      }
    } else {  // to end game
      return;
    }
  }
}

void RingClient::closeGame() {
  close(this->master_fd);
  close(this->right_fd);
  close(this->left_fd);
  close(this->fd);
}