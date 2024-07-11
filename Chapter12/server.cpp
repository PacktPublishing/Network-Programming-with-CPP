#include <iostream>
#include <experimental/net>

namespace net = std::experimental::net;

int main()
{
    net::io_context io_context;

    // Listen for new connection
    net::ip::tcp::acceptor acceptor(io_context, net::ip::tcp::endpoint(net::ip::tcp::v4(), 1234));
    // Creating socket & waiting for connection
    try
    {
        net::ip::tcp::socket socket = acceptor.accept(io_context);
        // Read operation
        std::string request;
        try
        {
            net::read( socket, net::buffer(request) );
        }
        catch (std::system_error e)
        {
            std::cout << "receive failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << "Server received message: " << request << std::endl;

        // Write back
        const std::string msg = "Hello to you too!\n";
        try
        {
            net::write( socket, net::buffer(msg) );
        }
        catch (std::system_error e)
        {
            std::cout << "send failed: " << e.what() << std::endl;
            return 1;
        }
        std::cout << "Server sent hello back!" << std::endl;
    }
    catch (std::system_error e)
    {
        std::cout << "connection failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

