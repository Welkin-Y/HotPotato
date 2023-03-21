#include "playerutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "utils.c"

extern void memErr();
extern int getServFd(const char* port);
extern int getConnFd(int sockfd, char** ip);
extern int getCliFd(const char* hostname, const char* port);

// Create player struct
RingPlayer* newPlayer() {
  RingPlayer* res = malloc(sizeof(RingPlayer));
  if (res == NULL) memErr();
  return res;
}

// free player struct
void destroyPlayer(RingPlayer* self) { free(self); }

// Connect to Ring master
void connectMaster(RingPlayer* self, const char* hostname, const char* port) {
  self->master_fd = getCliFd(hostname, port);
  recv(self->master_fd, &(self->id), sizeof(self->id), 0);
  recv(self->master_fd, &(self->num_players), sizeof(self->num_players), 0);
  self->fd = getServFd("");
  int player_port = getPort(self->fd);
  send(self->master_fd, &player_port, sizeof(player_port), 0);
  printf("Connected as player %d out of %d total players\n", self->id,
         self->num_players);
}

// Connect left neighbor
void connLeftPlayer(RingPlayer* self) {
  addr_t addr;
  recv(self->master_fd, &addr, sizeof(addr),
       MSG_WAITALL);  // recv (id-1)th IP:PORT
  char port[8] = {0};
  sprintf(port, "%d", addr.port);
  self->left_fd = getCliFd(addr.ip, port);
}

// Connect right neighbor
void connRightPlayer(RingPlayer* self) {
  char* right_ip;
  self->right_fd = getConnFd(self->fd, &right_ip);  // accept (id+1) connection
  free(right_ip);
}

// Connect neighbors
void connectPeers(RingPlayer* self) {
  connLeftPlayer(self);
  connRightPlayer(self);
}

// Send potato to neighbors
inline void sendToPeers(RingPlayer* self, Potato* potato) {
  int rand_int = rand() & 1;
  if (rand_int == 0) {
    send(self->left_fd, potato, sizeof(*potato), 0);
    int left_id = self->id == 0 ? self->num_players - 1 : self->id - 1;
    printf("Sending potato to %d\n", left_id);
  } else {
    send(self->right_fd, potato, sizeof(*potato), 0);
    int right_id = self->id + 1 == self->num_players ? 0 : self->id + 1;
    printf("Sending potato to %d\n", right_id);
  }
}

// Run game
void playGame(RingPlayer* self) {
  Potato potato;
  fd_set readfds;
  int fds[3] = {self->left_fd, self->right_fd, self->master_fd};
  int nfds = MAX(MAX(self->left_fd, self->right_fd), self->master_fd);
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
      potato.path[potato.cnt] = self->id;
      potato.cnt++;
      // if num hops == 0
      if (potato.num_hops != 0) {
        sendToPeers(self, &potato);
      } else {  // if I am it
        send(self->master_fd, &potato, sizeof(potato), 0);
        printf("I'm it\n");
        return;
      }
    } else {  // to end game
      return;
    }
  }
}

void closeGame(RingPlayer* self) {
  close(self->master_fd);
  close(self->right_fd);
  close(self->left_fd);
  close(self->fd);
  destroyPlayer(self);
}