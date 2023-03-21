all: ringmaster player
.PHONY : all
CC=gcc
CFLAGS=-O3 -fPIC

ringmaster:
	$(CC) $(CFLAGS) ringmaster.c -o ringmaster -g
player:
	$(CC) $(CFLAGS) player.c -o player -g

.PHONY:clean
clean:
	rm ringmaster player
	
