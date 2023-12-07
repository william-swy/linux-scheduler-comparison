BINARIES := test_gzip test_gzip_harness.o

.PHONY: all

all: ${BINARIES}

test_gzip: test_gzip_harness.o ../launcher/launcher.cpp
	$(MAKE) -C ../launcher
	g++ -std=c++17 -o test_gzip test_gzip_harness.o ../launcher/launcher.o

test_gzip_harness.o: test_gzip_harness.cpp ../launcher/test_prog_harness.h
	g++ -Wall -O2 -std=c++17 -I ../launcher -o test_gzip_harness.o -c test_gzip_harness.cpp

clean:
	rm -f ${BINARIES}
	$(MAKE) -C ../launcher clean
