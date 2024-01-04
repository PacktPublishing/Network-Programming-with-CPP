// g++ example_server.cpp -lnanomsg

#include <iostream>
#include <string>
#include <time.h>
#include <sys/time.h>
#include <cstring>
#include <arpa/inet.h>

#include <nanomsg/nn.h>
#include <nanomsg/reqrep.h>

#define MAXJOBS 100

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

int server(const std::string & url)
{
    Work *worklist = NULL;
    int npending = 0;

    int socket = nn_socket(AF_SP_RAW, NN_REP);
    if (socket < 0) 
    {
        std::cout << "nn_socket: " << nn_strerror(nn_errno()) << std::endl;
        return 1;
    }

    int ec = nn_bind(socket, url.c_str());

    if (ec < 0) 
    {
        std::cerr << "nn_bind: " << nn_strerror(nn_errno()) << std::endl;
        nn_close(socket);
        return 1;
    }

    while (true) 
    {
        uint32_t timer;
        int rc;
        int timeout;
        uint64_t now;
        Work *work;
        Work **srch;
        uint8_t *body;
        void *control;
        nn_iovec iov;
        nn_msghdr hdr;
        nn_pollfd pfd[1];

        timeout = -1;
        while ((work = worklist) != NULL) 
        {
            now = milliseconds();
            if (work->expire > now) {
                timeout = (work->expire - now);
                break;
            }
            worklist = work->next;
            npending--;
            rc = nn_sendmsg(socket, &work->request, NN_DONTWAIT);
            if (rc < 0) 
            {
                std::cerr << "nn_sendmsg: " << nn_strerror(nn_errno()) << std::endl;
                nn_freemsg(work->request.msg_control);
            }
            delete work;
        }

        if (npending >= MAXJOBS) 
        {
            nn_poll(pfd, 0, timeout);
            continue;
        }

        pfd[0].fd = socket;
        pfd[0].events = NN_POLLIN;
        pfd[0].revents = 0;
        nn_poll(pfd, 1, timeout);

        if ((pfd[0].revents & NN_POLLIN) == 0) 
        {
            continue;
        }

        memset(&hdr, 0, sizeof(hdr));
        control = NULL;
        iov.iov_base = &body;
        iov.iov_len = NN_MSG;
        hdr.msg_iov = &iov;
        hdr.msg_iovlen = 1;
        hdr.msg_control = &control;
        hdr.msg_controllen = NN_MSG;

        rc = nn_recvmsg(socket, &hdr, 0);
        if (rc < 0) 
        {
            std::cerr << "nn_recv: " << nn_strerror(nn_errno()) << std::endl;
            break;
        }
        if (rc != sizeof(uint32_t)) 
        {
            std::cerr << "nn_recv: wanted " << sizeof(uint32_t) << ", but got " << rc << std::endl,
            nn_freemsg(body);
            nn_freemsg(control);
            continue;
        }

        memcpy(&timer, body, sizeof(timer));
        nn_freemsg(body);

        std::cout << "Received query - sending response in " << ntohl(timer) << "ms" << std::endl ;
        work = new Work;
        if (work == NULL) 
        {
            std::cerr << "new: " << strerror(errno) << std::endl;
            break;
        }
        memset(work, 0, sizeof(*work));
        work->expire = milliseconds() + ntohl(timer);
        work->control = control;
        work->request.msg_iovlen = 0;  /*  No payload data to send. */
        work->request.msg_iov = NULL;
        work->request.msg_control = &work->control;
        work->request.msg_controllen = NN_MSG;

        srch = &worklist;
        while (true) 
        {
            if ((*srch == NULL) || ((*srch)->expire > work->expire)) 
            {
                work->next = *srch;
                *srch = work;
                npending++;
                break;
            }
            srch = &((*srch)->next);
        }
    }

    nn_close(socket);
    return 1;
}


int main()
{
    std::string url = "tcp://127.0.0.1:8080";
    int rc = server(url);
    
    return rc;
}
