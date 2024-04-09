CC = g++
CFLAGS = -lcurl

basic_example: basic_example.cpp
	$(CC) -o $@ $^ $(CFLAGS)

example_tcp: example_tcp.cpp
	$(CC) -o $@ $^ $(CFLAGS)

member_function_callback: member_function_callback.cpp
	$(CC) -o $@ $^ $(CFLAGS)

all: basic_example example_tcp member_function_callback


clean: 
	rm -f basic_example
	rm -f example_tcp
	rm -f member_function_callback

help:  
	cat ./README
