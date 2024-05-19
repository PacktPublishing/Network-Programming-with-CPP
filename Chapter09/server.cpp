#include <iostream>
#include <boost/network/protocol/http/server.hpp>


struct HelloWorld;
typedef boost::network::http::server<HelloWorld> Server;

struct HelloWorld
{
    void operator()(Server::request const &request, Server::connection_ptr connection) 
    {
        Server::string_type ip = source(request);
        unsigned int port = request.source_port;
        
        std::ostringstream data;
        data << "Hello, " << ip << ':' << port << '!';
        
        connection->set_status(Server::connection::ok);
        connection->write(data.str());
    }
};

int main(int argc, char *argv[]) 
{

    if (argc != 3) 
    {
        std::cout << "Usage: " << argv[0] << " address port" << std::endl;
        return 1;
    }

    std::string address = argv[1];
    std::string port = argv[2];
    
    try 
    {
        HelloWorld hw_handler;
        Server::options options(hw_handler);
        Server server(options.address(address).port(port));
        server.run();
    }
    catch (std::exception &e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

