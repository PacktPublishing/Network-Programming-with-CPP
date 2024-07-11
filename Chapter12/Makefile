CC = g++
CFLAGS = -Inetworking-ts-impl/include

server: server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

client: client.cpp
	$(CC) -o $@ $^ $(CFLAGS)


all: server client


clean: 
	rm -f server
	rm -f client

help:  
	cat ./README
