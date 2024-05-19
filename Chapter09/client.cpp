#include <iostream>
#include <string>
#include <sstream>
#include <boost/network/protocol/http/client.hpp>

namespace http = boost::network::http;

int main(int argc, char * argv[])
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
        std::ostringstream full_url;
        full_url << "http://" << address << ":" << port << "/";
        
        http::client::request request(full_url.str());
        
        http::client client;
        http::client::response response = client.get(request);
        
        std::cout << body(response) << std::endl;
    }
    catch (std::exception & e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

