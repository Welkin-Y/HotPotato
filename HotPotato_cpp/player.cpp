#include <cstdlib>
#include <iostream>

#include "ringclient.cpp"

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("usage: player <machine name> <port num>\n");
    return EXIT_FAILURE;
  }
  RingClient player = RingClient(argv[1], argv[2]);
  Potato potato = Potato();
  player.playGame(potato);
  return EXIT_SUCCESS;
}