#include <iostream>
#include <experimental/net>

namespace net = std::experimental::net;

int main()
{
    net::io_context io_context;
    net::ip::tcp::socket socket(io_context);
    net::ip::tcp::resolver resolver(io_context);
   
    try
    {
        net::connect(socket, resolver.resolve("127.0.0.1", "1234"));
    }
    catch (std::system_error e)
    {
        std::cout << "connection failed: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Client connected!" << std::endl;

    //  Request/message from client
    const std::string msg = "Hello from Client!\n";
    try
    {
        net::write( socket, net::buffer(msg) );
    }
    catch (std::system_error e)
    {
        std::cout << "send failed: " << e.what() << std::endl;
        return 1;
    }    
    std::cout << "Client sent hello message!" << std::endl;

    //  Getting response from server
    std::string response;
    try
    {
        net::read(socket, net::buffer(response) );
    }
    catch (std::system_error e)
    {
        std::cout << "receive failed: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "Client received message: " << response << std::endl;

    return 0;
}
