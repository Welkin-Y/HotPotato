#ifndef _ADDR_H_
#define _ADDR_H_
const int MAX_IP_LEN = 40;
struct addr_t {
  char ip[MAX_IP_LEN];
  int port;
} typedef addr_t;
#endif