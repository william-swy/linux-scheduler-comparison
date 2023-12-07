BINARIES := launcher.o

.PHONY: all
all: ${BINARIES}

launcher.o : launcher.cpp
	g++ -Wall -O2 -std=c++17 -o launcher.o -c launcher.cpp

clean:
	rm -f ${BINARIES}