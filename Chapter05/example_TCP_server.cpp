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

    std::cerr << "uv_write error: " << uv_strerror(status) << std::endl;

    if (status == UV_ECANCELED)
        return;

    uv_close(reinterpret_cast<uv_handle_t*>(request->handle), on_close);
}


static void after_shutdown(uv_shutdown_t* request, int status) 
{
    if (status < 0)
        std::cerr << "err: " << uv_strerror(status) << std::endl;
    std::cerr << "data received: " << (data_container / 1024 / 1024) << std::endl;
    data_container = 0;
    uv_close(reinterpret_cast<uv_handle_t*>(request->handle), on_close);
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
        uv_shutdown(request, handle, after_shutdown);
        return;
    }

    data_container += nread;
    write_request = new WriteRequest;
    write_request->buffer = uv_buf_init(buf->base, nread);
    uv_write(&write_request->request, handle, &write_request->buffer, 1, after_write);
}


static void alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buffer) 
{
    buffer->base = new char[suggested_size];
    buffer->len = suggested_size;
}


static void on_connection(uv_stream_t* server, int status) 
{
    uv_tcp_t* stream;
    stream = new uv_tcp_t;
    uv_tcp_init(uv_default_loop(), stream);
    stream->data = server;
    uv_accept(server, reinterpret_cast<uv_stream_t*>(stream));
    uv_read_start(reinterpret_cast<uv_stream_t*>(stream), alloc_cb, after_read);
}


static int tcp_echo_server() 
{
    uv_tcp_t* tcp_server;
    sockaddr_in addr;
    
    uv_ip4_addr("0.0.0.0", PORT, &addr);    
    tcp_server = new uv_tcp_t;    
    uv_tcp_init(uv_default_loop(), tcp_server);
    uv_tcp_bind(tcp_server, reinterpret_cast<sockaddr*>(&addr), 0);
    uv_listen(reinterpret_cast<uv_stream_t*>(tcp_server), SOMAXCONN, on_connection);
    
    return 0;
}


int main() 
{
    tcp_echo_server();

    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    return 0;
}
