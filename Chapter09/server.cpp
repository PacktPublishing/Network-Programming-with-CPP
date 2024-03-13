#include <boost/network/protocol/http/server.hpp>
#include <iostream>

namespace http = boost::network::http;

struct hello_world;
typedef http::server<hello_world> server;

struct hello_world {
    void operator()(server::request const &request, server::connection_ptr connection) {
        server::string_type ip = source(request);
        unsigned int port = request.source_port;
        std::ostringstream data;
        data << "Hello, " << ip << ':' << port << '!';
        connection->set_status(server::connection::ok);
        connection->write(data.str());
    }
};

int main(int argc, char *argv[]) {

    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " address port" << std::endl;
        return 1;
    }

    try {
        hello_world handler;
        server::options options(handler);
        server server_(options.address(argv[1]).port(argv[2]));
        server_.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}

