CC=gcc
CFLAGS=-std=gnu99 -g -Wall -O0
PROGRAMS=execcnts

default: execcnts

all: execcnts test

execcnts:
	$(CC) $(CFLAGS) $(PROGRAMS).c ../counts/getcounts.a -o execcnts
    
test: fork vfork clone execve
	$(CC) $(CFLAGS) $^ -o $@

fork: fork.o
	$(CC) $(CFLAGS) $^ -o $@

vfork: vfork.o
	$(CC) $(CFLAGS) $^ -o $@

clone: clone.o
	$(CC) $(CFLAGS) $^ -o $@

execve: execve.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c %.d
	$(CC) $(CFLAGS) -o $@ -c $<

%.d: %.c
	$(CC) $(CXXFLAGS) -MM -MT '$(patsubst %.c,%.o,$<)' $< -MF $@

clean:
	rm -f $(OBJS) $(PROGRAMS)
