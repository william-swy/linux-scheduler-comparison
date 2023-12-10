BINARIES := time_tracker sched_tracker

.PHONY: all
all: ${BINARIES}

time_tracker : time_tracker.cpp
	g++ -Wall -O2 -std=c++17 -o time_tracker time_tracker.cpp
	sudo setcap CAP_SYS_NICE+ep time_tracker

sched_tracker : sched_tracker.cpp
	g++ -Wall -O2 -std=c++17 -o sched_tracker sched_tracker.cpp
	sudo setcap CAP_SYS_NICE+ep sched_tracker

clean:
	rm -f ${BINARIES}