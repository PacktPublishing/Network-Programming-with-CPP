#include <iostream>
#include <uv.h>

#define PORT 8000

struct WriteRequest 
{
    uv_write_t request;
    uv_buf_t buffer;
};

static uint64_t data_container = 0;


static void on_close(uv_handle_t* handle) 
{
    delete handle;
}


static void after_write(uv_write_t* request, int status) 
{
    WriteRequest* write_request = (WriteRequest*)request;

    if (write_request->buffer.base != NULL)
        delete[] write_request->buffer.base;
    delete write_request;

    if (status == 0)
        return;

    std::cerr << "uv_write error: " <<uv_strerror(status) << std::endl;

    if (status == UV_ECANCELED)
        return;

    if (status != UV_EPIPE) { std::cerr << "status invalid: " << status << std::endl; return; }
    uv_close((uv_handle_t*)request->handle, on_close);
}


static void after_shutdown(uv_shutdown_t* request, int status) 
{
    if (status < 0)
        std::cerr << "err: " << uv_strerror(status) << std::endl;
    std::cerr << "data received: " << (data_container / 1024 / 1024) << std::endl;
    data_container = 0;
    uv_close((uv_handle_t*)request->handle, on_close);
    delete request;
}


static void after_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) 
{
    WriteRequest* write_request;
    uv_shutdown_t* request;

    if (nread <= 0 && buf->base != NULL)
        delete[] buf->base;

    if (nread == 0)
        return;

    if (nread < 0) 
    {
        std::cerr << "err: " << uv_strerror(nread) << std::endl;

        request = new uv_shutdown_t;
        if (request == NULL) { std::cerr << "new failed" << std::endl; return; }

        int ec = uv_shutdown(request, handle, after_shutdown);
        if (ec != 0) { std::cerr << "uv_shutdown failed with value " << ec << std::endl; return; }

        return;
    }

    data_container += nread;

    write_request = new WriteRequest;
    if (write_request == NULL) { std::cerr << "new failed" << std::endl; return; }

    write_request->buffer = uv_buf_init(buf->base, nread);

    int ec = uv_write(&write_request->request, handle, &write_request->buffer, 1, after_write);
    if (ec != 0) { std::cerr << "uv_write failed with value " << ec << std::endl; return; }
}


static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buffer) 
{
    buffer->base = new char[suggested_size];
    if (buffer->base == NULL) { std::cerr << "new failed" << std::endl; return; }
    buffer->len = suggested_size;
}


static void on_connection(uv_stream_t* server, int status) 
{
    uv_tcp_t* stream;

    if (status != 0) { std::cerr << "status not OK" << std::endl; return; }

    stream = new uv_tcp_t;
    if (stream == NULL) { std::cerr << "new failed" << std::endl; return; }

    int ec = uv_tcp_init(uv_default_loop(), stream);
    if (ec != 0) { std::cerr << "uv_tcp_init failed with value " << ec << std::endl; return; }

    stream->data = server;

    ec = uv_accept(server, (uv_stream_t*)stream);
    if (ec != 0) { std::cerr << "uv_accept failed with value " << ec << std::endl; return; }

    ec = uv_read_start((uv_stream_t*)stream, alloc_cb, after_read);
    if (ec != 0) { std::cerr << "uuv_read_startv_listen failed with value " << ec << std::endl; return; }
}


static int tcp_echo_server() 
{
    uv_tcp_t* tcp_server;
    struct sockaddr_in addr;
    
    int ec = uv_ip4_addr("0.0.0.0", PORT, &addr);
    if (ec != 0) { std::cerr << "uv_run failed with value " << ec << std::endl; return ec; }
    
    tcp_server = new uv_tcp_t;
    if (tcp_server == NULL) { std::cerr << "new failed" << std::endl; return 1; }
    
    ec = uv_tcp_init(uv_default_loop(), tcp_server);
    if (ec != 0) { std::cerr << "uv_tcp_init failed with value " << ec << std::endl; return ec; }
    
    ec = uv_tcp_bind(tcp_server, reinterpret_cast<sockaddr*>(&addr), 0);
    if (ec != 0) { std::cerr << "uv_tcp_bind failed with value " << ec << std::endl; return ec; }
    
    ec = uv_listen(reinterpret_cast<uv_stream_t*>(tcp_server), SOMAXCONN, on_connection);
    if (ec != 0) { std::cerr << "uv_listen failed with value " << ec << std::endl; return ec; }
    
    return 0;
}


int main() 
{
    int ec = tcp_echo_server();
    if (ec != 0) { std::cerr << "tcp_echo_server failed with value " << ec << std::endl; return ec; }

    ec = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    if (ec != 0) { std::cerr << "uv_run failed with value " << ec << std::endl; return ec; }

    return 0;
}
