CC=gcc
CFLAGS=-std=gnu99 -g -Wall -O0
PROGRAMS=execcnts

default: all

all: execcnts test

execcnts:
	$(CC) $(CFLAGS) $(PROGRAMS).c ../counts/getcounts.a -o execcnts

test: fork.o vfork.o clone.o execve.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.d
	$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	$(CC) $(CXXFLAGS) -MM -MT '$(patsubst %.c,%.o,$<)' $< -MF $@

clean:
	rm -f $(OBJS) $(PROGRAMS)
