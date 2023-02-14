#include <iostream>
#include <curl/curl.h>
 
// Function that is used to wait on a socket
static int wait(curl_socket_t sock, bool receive_mode, long timeout_ms)
{
    timeval tv; 
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    tv.tv_sec = timeout_ms / 1000;
 
    fd_set in_fd, out_fd, error_fd;
    FD_ZERO(&in_fd);
    FD_ZERO(&out_fd);
    FD_ZERO(&error_fd);

    FD_SET(sock, &error_fd);
 
    if(receive_mode)
        FD_SET(sock, &in_fd);
    else
        FD_SET(sock, &out_fd);
 
    // select return the number of activated sockets
    int nsockets = select(static_cast<int>(sock) + 1, &in_fd, &out_fd, &error_fd, &tv);
    return nsockets;
}
 
int main(void)
{
    CURL * curl;
    curl = curl_easy_init();
    const long TIMEOUT = 60000L;
    const size_t BUFF_SIZE = 1024;
 
    if(curl) 
    {
        // Setting the URL and the options

        curl_easy_setopt(curl, CURLOPT_URL, "https://belaycpp.com");
        curl_easy_setopt(curl, CURLOPT_CONNECT_ONLY, 1L);

        // Connection

        CURLcode ecode = curl_easy_perform(curl);
 
        if(ecode != CURLE_OK) 
        {
            std::cerr << "Error: " << curl_easy_strerror(ecode) << std::endl;
            return 1;
        }
 
        // Getting info on the socket

        curl_socket_t sock;
        ecode = curl_easy_getinfo(curl, CURLINFO_ACTIVESOCKET, &sock);
 
        if(ecode != CURLE_OK) 
        {
            std::cerr << "Error: " << curl_easy_strerror(ecode) << std::endl;
            return 1;
        }
 
        // Sending request
        std::cout << "Sending request..." << std::endl;
 
        std::string request = "GET / HTTP/1.0\r\nHost: belaycpp.com\r\n\r\n";
        size_t nsent_total = 0;

        // Looping in case the message is fragmented
        do 
        {
            size_t nsent = 0;
            do 
            {
                nsent = 0;
                ecode = curl_easy_send(curl, request.c_str() + nsent_total, request.size() - nsent_total, &nsent);
                nsent_total += nsent;
 
                if(ecode == CURLE_AGAIN && wait(sock, false, TIMEOUT) == 0) 
                {
                    std::cerr << "Error: timeout\n" << std::endl;
                    return 1;
                }
            } while(ecode == CURLE_AGAIN); // "AGAIN" means we have to send the message again
 
            if(ecode != CURLE_OK) 
            {
                std::cerr << "Error: %s\n" << curl_easy_strerror(ecode) << std::endl;
                return 1;
            }
 
            printf("Sent %lu bytes.\n", static_cast<unsigned long>(nsent));
 
        } while(nsent_total < request.size());
 
        // Reading response
        std::cout << "Reading response..." << std::endl;
 
        // Looping until we receive no more
        bool reach_end = false;
        while(!reach_end) 
        {
            char buffer[BUFF_SIZE];
            size_t nread;
            do 
            {
                nread = 0;
                ecode = curl_easy_recv(curl, buffer, sizeof(buffer), &nread);
 
                if(ecode == CURLE_AGAIN && wait(sock, true, TIMEOUT) == 0) 
                {
                    std::cerr << "Error: timeout" << std::endl;
                    return 1;
                }
            } while(ecode == CURLE_AGAIN);
 
            if(ecode != CURLE_OK)
            {
                std::cerr << "Error: " << curl_easy_strerror(ecode) << std::endl;
                return 1;
            }
 
            if(nread == 0)
                reach_end = true;
            else
                std::cout <<"Received " << nread << " bytes" << std::endl;
        }
 
        // Cleanup

        curl_easy_cleanup(curl);
    }
    return 0;
}
