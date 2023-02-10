#include <iostream>
#include <curl/curl.h>
 
int main()
{
    CURL * curl;
    CURLcode ecode;
    CURLU * url;
    CURLUcode ucode;
    char * host;
    char * path;

    curl = curl_easy_init();

    // Gets an url
    url = curl_url();
    if(!url)
        return 1;
 
    // Parse an URL
    ucode = curl_url_set(url, CURLUPART_URL, "https://belaycpp.com/team/", 0);
    if(ucode != CURLUE_OK)
    {
        std::cerr << "Error on URL set [" << ucode << "]" << std::endl;
        curl_url_cleanup(url);
        return 1;
    }
 
    // Get the host name from the URL
    ucode = curl_url_get(url, CURLUPART_HOST, &host, 0);
    if(ucode == CURLUE_OK) 
    {
        std::cout << "Host name: " << host << std::endl;
        curl_free(host);
    }
 
    // Gets the path from the URL
    ucode = curl_url_get(url, CURLUPART_PATH, &path, 0);
    if(ucode == CURLUE_OK) 
    {
        std::cout << "Path: " << path << std::endl;
        curl_free(path);
    }
 
    curl_easy_setopt(curl, CURLOPT_CURLU, url);
    ecode = curl_easy_perform(curl);
    if(ecode != CURLE_OK)
    {
        std::cerr << "Error on perform [" << ecode << "]" << std::endl;
        curl_url_cleanup(url);
        return 1;
    }
 
    return 0;
}
