CC=gcc
CFLAGS=-std=gnu99 -g -Wall -O0
PROGRAMS=execcnts

default: all

all: 
	$(CC) $(CFLAGS) $(PROGRAM).c ../counts/getcounts.a -o execcnts

clean:
	rm -f $(OBJS) $(PROGRAMS)
