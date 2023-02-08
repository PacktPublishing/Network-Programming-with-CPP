#include <iostream>
#include <string>
#include <cstring>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

void cleanup(SSL_CTX* context, BIO* bio)
{
    SSL_CTX_free(context);
    BIO_free_all(bio);
}

int main() 
{
    std::string hostname = "belaycpp.com:443";

    // Init

    SSL_load_error_strings();
    SSL_library_init();
    
    // Context creation

    const SSL_METHOD* method = TLS_client_method();
    if (method == nullptr)
        return 1;

    SSL_CTX* context = SSL_CTX_new(method);
    if (context == nullptr)
        return 1;

    BIO* bio = BIO_new_ssl_connect(context);
    if (bio == nullptr)
        return 1;

    SSL* ssl = NULL;

    // Connection

    std::string name = hostname + ":https";
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);
    BIO_set_conn_hostname(bio, name.c_str());
    int success = BIO_do_connect(bio);

    if (success != 1) 
    {
        cleanup(context, bio);
        std::cerr << "Connection failed" << std::endl;
        return 1;
    }

    // Certificate validation

    success = SSL_CTX_load_verify_locations(context, "/etc/ssl/certs/ca-certificates.crt", "/etc/ssl/certs/");
    if (success != 1)
    {
        cleanup(context, bio);
        std::cerr << "Certificate verification failed" << std::endl;
        return 1;
    }

    long verify_flag = SSL_get_verify_result(ssl);
    if (verify_flag != X509_V_OK)
        std::cerr << "Certificate verification error: " << verify_flag << std::endl;

    // Request

    std::string request = "GET / HTTP/1.1\x0D\x0AHost: "+hostname+"\x0D\x0A\x43onnection: Close\x0D\x0A\x0D\x0A";
    BIO_puts(bio, request.c_str());

    const size_t BUFF_SIZE = 1024;
    char response[BUFF_SIZE];
    while (1) 
    {
        memset(response, 0, sizeof(response));
        int nread = BIO_read(bio, response, BUFF_SIZE);
        if (nread <= 0)
            break;
        std::cout << response;
    }
    std::cout << std::endl;

    // Cleanup

    cleanup(context, bio);

    return 0;
}
