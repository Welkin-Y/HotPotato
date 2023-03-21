#include "ringmasterutils.c"

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
  RingMaster* master = newMaster(argv[1], num_players, num_hops);
  Potato potato = {master->num_hops, 0, {0}};
  initGame(master);
  runGame(master, &potato);
  printResult(&potato);
  closeGame(master, &potato);

  return EXIT_SUCCESS;
}