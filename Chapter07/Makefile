CC=g++
CFLAGS=/usr/lib/x86_64-linux-gnu/libACE.so

all: example_server example_client


clean:
	rm -f client
	rm -f server


server: example_server.cpp server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

client: example_client.cpp client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

	
