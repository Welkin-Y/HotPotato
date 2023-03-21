#ifndef _POTATO_H_
#define _POTATO_H_
#define MAX_PATH_LEN 512

// Potato struct
struct Potato_t {
  int num_hops;
  int cnt;
  int path[MAX_PATH_LEN];
} typedef Potato;
#endif
