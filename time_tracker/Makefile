BINARIES := time_tracker

.PHONY: all
all: ${BINARIES}

time_tracker : time_tracker.cpp
	g++ -Wall -O2 -std=c++17 -o time_tracker time_tracker.cpp

clean:
	rm -f ${BINARIES}