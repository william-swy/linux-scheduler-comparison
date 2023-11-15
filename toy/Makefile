PROGS := launcher hackbench fib

.PHONY: all
all: ${PROGS}

hackbench : hackbench.c
	gcc -g -Wall -O2 -o hackbench hackbench.c -lpthread

launcher : launcher.c
	gcc -Wall -O2 -o launcher launcher.c

fib : fib.cpp
	g++ -Wall -O2 -o fib fib.cpp

clean:
	rm -f ${PROGS}