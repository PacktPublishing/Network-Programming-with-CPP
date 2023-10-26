CC = g++
CFLAGS = -isystem -lboost_system 


tcp_server: example_asio_tcp_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp_client: example_asio_tcp_client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp: tcp_server tcp_client 


udp_server: example_asio_udp_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)


udp_client: example_asio_udp_client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

udp: udp_server udp_client


all: tcp_server tcp_client udp_server udp_client


clean: 
	rm -f tcp_server
	rm -f tcp_client
	rm -f udp_server
	rm -f udp_client

help:  
	cat ./README
