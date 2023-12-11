BINARIES := seed_db query_test

.PHONY: all

all: ${BINARIES}

seed_db: seed_db.cpp
	g++ -std=c++17 seed_db.cpp -o seed_db -I /usr/include/postgresql -L /usr/lib/x86_64-linux-gnu -l:libpq.so

query_test: query_test.cpp
	g++ -std=c++17 query_test.cpp -o query_test -I /usr/include/postgresql -L /usr/lib/x86_64-linux-gnu -l:libpq.so

clean:
	rm -f ${BINARIES}