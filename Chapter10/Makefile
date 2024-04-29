CC = g++
CFLAGS = -fPIC -lQt5Core -lQt5Network -I /usr/include/x86_64-linux-gnu/qt5/ -I /usr/include/x86_64-linux-gnu/qt5/QtCore/ -I /usr/include/x86_64-linux-gnu/qt5/QtNetwork/


tcp_server: example_qt_tcp_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp_client: example_qt_tcp_client.cpp
	$(CC) -o $@ $^ $(CFLAGS)

tcp: tcp_server tcp_client 


udp_server: example_qt_udp_server.cpp
	$(CC) -o $@ $^ $(CFLAGS)

udp_client: example_qt_udp_client.cpp
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
