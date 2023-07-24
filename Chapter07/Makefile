all: example_server example_client


clean:
	rm *.o


example_server: example_server.o server.o
	g++ -o example_server example_server.o server.o

example_server.o: example_server.cpp server.cpp
	g++ -o example_server.o example_server.cpp server.cpp -Wall

	
example_client: example_client.o server.o
	g++ -o example_client example_client.o client.o

example_client.o: example_client.cpp client.cpp
	g++ -o example_client.o example_client.cpp client.cpp -Wall

	
