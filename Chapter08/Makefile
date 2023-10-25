CC = g++
CFLAGS = -isystem -lboost_system 


%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

tcp_server: example_asio_tcp_server.o example_asio_tcp_server.cpp 
	$(CC) -c -o $@ $^ $(CFLAGS)

tcp_client: example_asio_tcp_client.o example_asio_tcp_client.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)


udp_server: example_asio_udp_server.o example_asio_udp_server.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)


udp_client: example_asio_udp_client.o example_asio_udp_client.cpp
	$(CC) -c -o $@ $^ $(CFLAGS)


all: tcp_server tcp_client udp_server udp_client


clean: 
	rm *.o 2> /dev/null
	rm tcp_server 2> /dev/null
	rm tcp_client 2> /dev/null
	rm udp_server 2> /dev/null
	rm udp_client 2> /dev/null

help:  
	cat ./README
