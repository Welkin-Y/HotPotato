#ifndef _POTATO_H_
#define _POTATO_H_
const int MAX_PATH_LEN = 512;

struct Potato {
  int num_hops;
  int cnt;
  int path[MAX_PATH_LEN];
};
#endif
