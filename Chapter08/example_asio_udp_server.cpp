
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>


const size_t MAX_LENGTH = 1024;

int main()
{
    try
    {
        boost::asio::io_context io_context;
        
        boost::asio::ip::udp::socket sock(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 1234));
        
        char data[MAX_LENGTH];
        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t length = sock.receive_from(boost::asio::buffer(data, MAX_LENGTH), sender_endpoint);

        std::cout << "Client sent ";
        std::cout.write(data, length);
        std::cout << "\n";
        
        sock.send_to(boost::asio::buffer(data, length), sender_endpoint);

        std::cout << "Server sent hello to client!" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
