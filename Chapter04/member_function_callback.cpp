#include <curl/curl.h>

struct Foo
{
    // Function we want to "set" as callback
    int my_read(char *buffer, size_t size, size_t nitems) { return 0; }

    // Wraping the f function in a static member funciton
    static int static_f(char *buffer, size_t size, size_t nitems, void *userdata)
    {
        return static_cast<Foo*>(userdata)->my_read(buffer, size, nitems);
    }
};


int main()
{
    CURLU * handle = curl_url();
    if(!handle)
        return 1;

    Foo foo;

    // Here, we pass our member function as a callback
    // using the static function that wraps the function my_read
    curl_easy_setopt(handle, CURLOPT_READFUNCTION, Foo::static_f);
    curl_easy_setopt(handle, CURLOPT_XFERINFODATA, &foo);
    return 0;
}
