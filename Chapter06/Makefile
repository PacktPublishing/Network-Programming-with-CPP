CC = g++
CFLAGS = -lnanomsg

server: example_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

client: example_client.cpp
	$(CC) -o $@ $^ $(CFLAGS)


all: server client


clean: 
	rm -f server
	rm -f client

help:  
	cat ./README
