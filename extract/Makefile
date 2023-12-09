BINARIES := test_extract test_extract_harness.o

.PHONY: all

all: ${BINARIES}

test_extract: test_extract_harness.o ../launcher/launcher.cpp
	$(MAKE) -C ../launcher
	g++ -std=c++17 -o test_extract test_extract_harness.o ../launcher/launcher.o

test_extract_harness.o: test_extract_harness.cpp ../launcher/test_prog_harness.h
	g++ -Wall -O2 -std=c++17 -I ../launcher -o test_extract_harness.o -c test_extract_harness.cpp

clean:
	rm -f ${BINARIES}
	$(MAKE) -C ../launcher clean
