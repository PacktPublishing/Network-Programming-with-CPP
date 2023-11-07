#include <iostream>
#include <boost/asio.hpp>

int main() 
{
    boost::asio::io_service io_service;
    // Listen for new connection
    boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 1234 ));
    // Socket creation
    boost::asio::ip::tcp::socket socket(io_service);
    // Waiting for connection
    acceptor.accept(socket);
    // Read operation
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "\n" );
    std::string message = boost::asio::buffer_cast<const char*>(buf.data());
    std::cout << message << std::endl;
    // Write operation
    boost::asio::write( socket, boost::asio::buffer("Hello From Server!") );
    std::cout << "Server sent Hello message to Client!" << std::endl;
    return 0;
}
