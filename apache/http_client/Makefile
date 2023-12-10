http_client: http_client.cpp
	g++ -Wall -O2 -std=c++17 -o http_client http_client.cpp
	sudo setcap CAP_SYS_NICE+ep http_client

clean:
	rm -rf http_client