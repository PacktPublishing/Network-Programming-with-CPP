#include <iostream>
#include <cstring> 

#include <uv.h>

uv_loop_t *loop;
uv_udp_t send_socket;
uv_udp_t recv_socket;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buffer_out) 
{
    *buffer_out = uv_buf_init(new char[suggested_size], suggested_size);
}

void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t * buf, const sockaddr *addr, unsigned int flags) 
{
    if (nread == -1) 
    {
        std::cerr << "Read error!" << std::endl;
        uv_close(reinterpret_cast<uv_handle_t*>(req), NULL);
        delete[] buf->base;
        return;
    }

    char sender[17] = { 0 };
    uv_ip4_name(reinterpret_cast<const sockaddr_in*>(addr), sender, 16);
    std::cerr << "Recv from " << sender << std::endl;

    // DHCP specific code
    unsigned int *as_integer = reinterpret_cast<unsigned int*>(buf->base);
    unsigned int ipbin = ntohl(as_integer[4]);
    unsigned char ip[4] = {0};
    int i;
    for (i = 0; i < 4; i++)
        ip[i] = (ipbin >> i*8) & 0xff;
    std::cerr << "Offered IP " << ip[3] << "." << ip[2] << "." << ip[1] << "." << ip[0] << std::endl;

    delete[] buf->base;
    uv_udp_recv_stop(req);
}

uv_buf_t make_msg(uv_udp_send_t *req) 
{
        uv_buf_t buffer;
        alloc_buffer(reinterpret_cast<uv_handle_t*>(req), 256, &buffer);
        memset(buffer.base, 0, buffer.len);

        // ... fill buffer with request

        return buffer;
}

void on_send(uv_udp_send_t *req, int status) 
{
    if (status == -1) 
    {
        std::cerr << "Send error!" << std::endl;
        return;
    }
}

int main() 
{
    loop = uv_default_loop();

    uv_udp_init(loop, &recv_socket);
    sockaddr_in recv_addr;
    uv_ip4_addr("0.0.0.0", 68, &recv_addr);
    uv_udp_bind(&recv_socket, reinterpret_cast<sockaddr*>(&recv_addr), 0);
    uv_udp_recv_start(&recv_socket, alloc_buffer, on_read);

    uv_udp_init(loop, &send_socket);
    sockaddr_in bc_addr;
    uv_ip4_addr("0.0.0.0", 0, &bc_addr);
    uv_udp_bind(&send_socket, reinterpret_cast<sockaddr*>(&bc_addr), 0);
    uv_udp_set_broadcast(&send_socket, 1);

    uv_udp_send_t send_req;
    uv_buf_t discover_msg = make_msg(&send_req);

    sockaddr_in send_addr;
    uv_ip4_addr("255.255.255.255", 67, &send_addr);
    uv_udp_send(&send_req, &send_socket, &discover_msg, 1, reinterpret_cast<sockaddr*>(&send_addr), on_send);

    return uv_run(loop, UV_RUN_DEFAULT);
}
