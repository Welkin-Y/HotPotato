#include "playerutils.c"

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("usage: player <machine name> <port num>\n");
    return EXIT_FAILURE;
  }
  RingPlayer* player = newPlayer();
  connectMaster(player, argv[1], argv[2]);
  connectPeers(player);
  playGame(player);
  closeGame(player);
  return EXIT_SUCCESS;
}