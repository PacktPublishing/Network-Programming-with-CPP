CC = g++
CFLAGS = -luv

all: dns tcp_client tcp_server tcp_server_fullerrorcheck udp

clean: 
	 rm -f dns
	 rm -f tcp_client
	 rm -f tcp_server
	 rm -f tcp_server_fullerrorcheck
	 rm -f udp


dns: example_DNS.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp_client: example_TCP_client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp_server: example_TCP_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp_server_fullerrorcheck: example_TCP_server_fullerrorcheck.cpp
	$(CC) -o $@ $^ $(CFLAGS)

udp: example_UDP.cpp
	$(CC) -o $@ $^ $(CFLAGS)
