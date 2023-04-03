#include <iostream>
#include <uv.h>

static uv_loop_t *loop;

static void alloc_cb(uv_handle_t* handle, size_t size, uv_buf_t* buf) 
{
    *buf = uv_buf_init(new char[size], size);
}

void on_close(uv_handle_t* handle)
{
    std::cout << "Closed." << std::endl;
}

void on_write(uv_write_t* req, int status)
{
    if (status)
    {
        std::cerr << "uv_write error " << std::endl;
        return;
    }
    std::cout << "wrote." << std::endl;
    delete req;
}

void on_read(uv_stream_t* tcp, ssize_t nread, const uv_buf_t* buf)
{
    std::cout << "on_read - " << tcp << std::endl;
    if(nread >= 0)
        std::cout << "Read: " << buf->base << std::endl;
    else
        uv_close(reinterpret_cast<uv_handle_t*>(tcp), on_close);

    delete[] buf->base;
}

void write2(uv_stream_t* stream, char *data, int len2) 
{
    uv_buf_t buffer[] =
    {
        {.base = data, .len = len2}
    };

    uv_write_t *req = new uv_write_t;
    uv_write(req, stream, buffer, 1, on_write);
}

void on_connect(uv_connect_t* connection, int status)
{
    if (status < 0)
    {
        std::cout << "Failed to connect" << std::endl;
        return;
    }
    std::cout << "connected - " << connection << ":" << status << std::endl;

    uv_stream_t* stream = connection->handle;
    delete connection;
    write2(stream, "echo    world!", 12);
    uv_read_start(stream, alloc_cb, on_read);
}

void start_connection(char *host, int port)
{
    uv_tcp_t *pSock = new uv_tcp_t;
    uv_tcp_init(loop, pSock);
    uv_tcp_keepalive(pSock, 1, 60);

    struct sockaddr_in dest;
    uv_ip4_addr(host, port, &dest);

    uv_connect_t *pConn = new uv_connect_t;
    std::cout <<  "allocated " << pConn << std::endl;
    uv_tcp_connect(pConn, pSock, reinterpret_cast<const sockaddr*>(&dest), on_connect);
}

int main(int argc, char **argv)
{
    loop = uv_default_loop();
    int i;
    for (i=0; i<10; i++)
        start_connection("127.0.0.1", 80);

    uv_run(loop, UV_RUN_DEFAULT);
}
