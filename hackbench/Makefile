BINARIES := hackbench

.PHONY: all
all: ${BINARIES}

hackbench : hackbench.c
	gcc -g -Wall -O2 -o hackbench hackbench.c

clean:
	rm -f ${BINARIES}