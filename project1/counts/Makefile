all: 
	gcc -c getexeccounts.c
	ar r getcounts.a getexeccounts.o
	gcc -c getDriver.c
	gcc getDriver.o getcounts.a

clean:
	rm *.a
	rm *.o
	rm *.out