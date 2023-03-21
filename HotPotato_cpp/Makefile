all: ringmaster player
.PHONY : all
CC=g++
CFLAGS=-O3 -fPIC

ringmaster:
	$(CC) $(CFLAGS) ringmaster.cpp -o ringmaster -g
player:
	$(CC) $(CFLAGS) player.cpp -o player -g

.PHONY:clean
clean:
	rm ringmaster player
	
