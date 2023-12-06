PROGS := launcher hackbench

.PHONY: all
all: ${PROGS}

hackbench : hackbench.c
	gcc -g -Wall -O2 -o hackbench hackbench.c -lpthread

launcher : launcher.cpp
	g++ -Wall -O2 -std=c++17 -o launcher launcher.cpp

clean:
	rm -f ${PROGS}