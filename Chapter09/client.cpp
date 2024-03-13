#include <boost/network/protocol/http/client.hpp>
#include <string>
#include <sstream>
#include <iostream>

namespace http = boost::network::http;

int main(int argc, char * argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " address port" << std::endl;
        return 1;
    }

    try {
        http::client client;
        std::ostringstream url;
        url << "http://" << argv[1] << ":" << argv[2] << "/";
        http::client::request request(url.str());
        http::client::response response =
            client.get(request);
        std::cout << body(response) << std::endl;
    } catch (std::exception & e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

