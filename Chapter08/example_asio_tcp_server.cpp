#include <iostream>
#include <boost/asio.hpp>

std::string read_(asio::ip::tcp::socket & socket) 
{
    asio::streambuf buf;
    asio::read_until( socket, buf, "\n" );
    std::string data = asio::buffer_cast<const char*>(buf.data());
    return data;
}
void send_(asio::ip::tcp::socket & socket, const std::string& message) 
{
    const std::string msg = message + "\n";
    asio::write( socket, asio::buffer(message) );
}

int main() 
{
    asio::io_service io_service;
    // Listen for new connection
    asio::ip::tcp::acceptor acceptor_(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 1234 ));
    // Socket creation
    asio::ip::tcp::socket socket_(io_service);
    // Waiting for connection
    acceptor_.accept(socket_);
    // Read operation
    std::string message = read_(socket_);
    cout << message << endl;
    // Write operation
    send_(socket_, "Hello From Server!");
    std::cout << "Servent sent Hello message to Client!" << std::endl;
   return 0;
}
