#include <iostream>
#include <cstring>
#include <uv.h>

uv_loop_t *loop;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t * buffer) 
{
    *buffer = uv_buf_init(new char[suggested_size], suggested_size);
}

void on_read(uv_stream_t *client, ssize_t nread, const uv_buf_t * buf) 
{
    if (nread == -1) 
    {
        std::cerr << "read failed error" << std::endl;
        uv_close(reinterpret_cast<uv_handle_t*>(client), NULL);
        delete client;
        return;
    }

    char *data = new char[nread+1];
    data[nread] = '\0';
    strncpy(data, buf->base, nread);

    std::cerr << data << std::endl;
    delete[] data;
    delete[] buf->base;
}


void on_connect(uv_connect_t *req, int status) 
{
    if (status == -1) 
    {
        std::cerr << "connect failed error" << std::endl;
        return;
    }

    uv_read_start(reinterpret_cast<uv_stream_t*>(req->data), alloc_buffer, on_read);
    delete req;
}

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

    uv_connect_t *connect_req = new uv_connect_t;
    uv_tcp_t *socket = new uv_tcp_t;
    uv_tcp_init(loop, socket);

    connect_req->data = socket;
    uv_tcp_connect(connect_req, socket, res->ai_addr, on_connect);

    uv_freeaddrinfo(res);
}

int main() 
{
    loop = uv_default_loop();

    struct addrinfo hints;
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
