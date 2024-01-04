// g++ example_client.cpp -lnanomsg

#include <iostream>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <cstring>
#include <arpa/inet.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

struct Work 
{
    Work *next;
    nn_msghdr request;
    uint64_t expire;
    void *control;
};

uint64_t milliseconds(void)
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return (static_cast<uint64_t>(tv.tv_sec * 1000) + static_cast<uint64_t>(tv.tv_usec / 1000));
}

int client(const std::string & url, unsigned int msec)
{
    int fd;
    int rc;
    char *greeting;
    uint8_t msg[sizeof(uint32_t)];
    uint64_t start;
    uint64_t end;

    fd = nn_socket(AF_SP, NN_REQ);
    if (fd < 0) 
    {
        std::cerr << "nn_socket: " << nn_strerror(nn_errno()) << std::endl;
        return 1;
    }

    if (nn_connect(fd, url.c_str()) < 0) 
    {
        std::cerr << "nn_socket: " << nn_strerror(nn_errno()) << std::endl;
        nn_close(fd);
        return 1;
    }

    msec = htonl(msec);
    memcpy(msg, &msec, sizeof(msec));

    start = milliseconds();

    std::cout << "Sending query, expecting a response in " << ntohl(msec) << "ms" << std::endl;  
    if (nn_send(fd, msg, sizeof(msg), 0) < 0) 
    {
        std::cerr << "nn_send: " << nn_strerror(nn_errno()) << std::endl;
        nn_close(fd);
        return 1;
    }

    rc = nn_recv(fd, &msg, sizeof(msg), 0);
    if (rc < 0) 
    {
        std::cerr << "nn_recv: " << nn_strerror(nn_errno()) << std::endl;
        nn_close(fd);
        return 1;
    }

    nn_close(fd);

    end = milliseconds();

    std::cout << "Request took " << (end - start) << " milliseconds." << std::endl;
    return 0;
}

int main()
{
    std::string url = "tcp://127.0.0.1:8080";
    unsigned int msec = 1500;
    int rc = client(url, msec);
    
    return rc;
}
