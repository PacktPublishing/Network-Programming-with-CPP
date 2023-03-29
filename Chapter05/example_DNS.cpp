#include <iostream>
#include <cstring>
#include <uv.h>



void on_resolved(uv_getaddrinfo_t *resolver, int status, struct addrinfo *res) 
{
    if (status == -1) 
    {
        std::cerr << "getaddrinfo callback error" << std::endl;
        return;
    }

    char addr[17] = {'\0'};
    uv_ip4_name(reinterpret_cast<sockaddr_in*>(res->ai_addr), addr, 16);
    std::cerr <<  addr << std::endl;

    // Connect and do stuff

    uv_freeaddrinfo(res);
}

int main() 
{
    uv_loop_t *loop = uv_default_loop();

    addrinfo hints;
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = 0;

    uv_getaddrinfo_t resolver;
    std::cerr << "belaycpp.com is... ";
    int r = uv_getaddrinfo(loop, &resolver, on_resolved, "belaycpp.com", "80", &hints);
     
    if (r) 
    {
        std::cerr << "getaddrinfo call error" << std::endl;
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}
