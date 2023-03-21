#ifndef _PLAYER_INFO_H_
#define _PLAYER_INFO_H_
#include "addr.h"
struct PlayerInfo {
  int fd;
  addr_t addr;
};
#endif