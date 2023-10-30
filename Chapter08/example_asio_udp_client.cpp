
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>


const size_t MAX_LENGTH = 1024;

int main()
{
    try
    {
        boost::asio::io_context io_context;

        boost::asio::ip::udp::socket s(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0));

        boost::asio::ip::udp::resolver resolver(io_context);
        boost::asio::ip::udp::resolver::results_type endpoints =
            resolver.resolve(boost::asio::ip::udp::v4(), "127.168.0.1", "1234");

        char request[] = "Hello world!";
        size_t request_length = std::strlen(request);
        s.send_to(boost::asio::buffer(request, request_length), *endpoints.begin());
        std::cout << "Client sent hello message!" << std::endl;

        char reply[MAX_LENGTH];
        boost::asio::ip::udp::endpoint sender_endpoint;
        size_t reply_length = s.receive_from(
                boost::asio::buffer(reply, MAX_LENGTH), sender_endpoint);
        std::cout << "Reply is ";
        std::cout.write(reply, reply_length);
        std::cout << "\n";
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
