CC=g++
CFLAGS=/usr/lib/x86_64-linux-gnu/libACE.so

all: server client


clean:
	rm -f client
	rm -f server


server: server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

client: client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

	
