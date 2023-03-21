#include "ringserver.cpp"

// check if args are valid
void checkArgs(int num_players, int num_hops) {
  if (num_players < 2) {
    fprintf(stderr, "Expect num players larger than 1\n");
    exit(EXIT_FAILURE);
  }
  if (num_hops < 0 || num_hops > MAX_PATH_LEN) {
    fprintf(stderr, "Expect num hops in [0,512]\n");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    printf("usage: ringmaster <port num> <num players> <num hops>\n");
    return EXIT_FAILURE;
  }
  int num_players = atoi(argv[2]);
  int num_hops = atoi(argv[3]);
  checkArgs(num_players, num_hops);
  RingServer master = RingServer(argv[1], num_players, num_hops);
  master.runGame();

  return EXIT_SUCCESS;
}