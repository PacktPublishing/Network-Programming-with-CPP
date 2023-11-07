#include "ace/SOCK_Acceptor.h"
#include "ace/Event_Handler.h"
#include "ace/Event_Handler.h"
#include "ace/INET_Addr.h"
#include "ace/SOCK_Stream.h"
#include "ace/Log_Msg.h"
#include "ace/Reactor.h"
ACE_Reactor *g_reactor;


class Logging_Handler : public ACE_Event_Handler
{
public:
    int open (ACE_Reactor *reactor)
    {
        reactor_ = reactor;

        if (reactor->register_handler (this, ACE_Event_Handler::READ_MASK) == -1)
            ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) can't register with reactor\n"), -1);
        return 0;
    }

    int close (void)
    {
        return this->handle_close (ACE_INVALID_HANDLE, ACE_Event_Handler::RWE_MASK);
    }

    operator ACE_SOCK_Stream &()
    {
        return this->cli_stream_;
    }

protected:

    ACE_HANDLE get_handle (void) const
    {
        return this->cli_stream_.get_handle ();
    }


    virtual int handle_input (ACE_HANDLE)
    {
        char buf[BUFSIZ + 1];

        ssize_t retval;
        switch (retval = this->cli_stream_.recv (buf, BUFSIZ))
        {
        case -1:
            ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) %p bad read\n", "client logger"), -1);
        case 0:
            ACE_ERROR_RETURN ((LM_ERROR, "(%P|%t) closing log daemon (fd = %d)\n", this->get_handle ()), -1);
        default:
            buf[retval] = '\0';
            ACE_DEBUG ((LM_DEBUG, "(%P|%t) from client: %s", buf));
        }

        return 0;
    }

    int handle_close (ACE_HANDLE, ACE_Reactor_Mask _mask)
    {
        reactor_->remove_handler (this, _mask | ACE_Event_Handler::DONT_CALL);
        cli_stream_.close ();
        delete this;

        return 0;
    }

protected:
    ACE_SOCK_Stream cli_stream_;
    ACE_Reactor *reactor_;
};


class Logging_Acceptor : public ACE_Event_Handler
{
public:
    int open (const ACE_INET_Addr &addr, ACE_Reactor *reactor)
    {
        if (this->peer_acceptor_.open (addr, 1) == -1)
            return -1;

        reactor_ = reactor;

        return reactor->register_handler (this, ACE_Event_Handler::ACCEPT_MASK);
    }

private:
    ACE_HANDLE get_handle (void) const
    {
        return this->peer_acceptor_.get_handle ();
    }

    virtual int handle_input (ACE_HANDLE handle)
    {
        ACE_UNUSED_ARG (handle);
        Logging_Handler *svc_handler;
        ACE_NEW_RETURN (svc_handler,Logging_Handler, -1);

        if (this->peer_acceptor_.accept (*svc_handler) == -1)
            ACE_ERROR_RETURN ((LM_ERROR, "%p", "accept failed"), -1);

        if (svc_handler->open (reactor_) == -1)
            svc_handler->close ();

        return 0;
    }

protected:
    ACE_SOCK_Acceptor peer_acceptor_;
    ACE_Reactor *reactor_;
};


static const u_short PORT = ACE_DEFAULT_SERVER_PORT;

int main (int, char *[])
{
    ACE_NEW_RETURN (g_reactor,ACE_Reactor,1);
    ACE_INET_Addr addr (PORT);
    Logging_Acceptor *peer_acceptor;
    ACE_NEW_RETURN (peer_acceptor,Logging_Acceptor,1);

    if (peer_acceptor->open (addr, g_reactor) == -1 )
        ACE_ERROR_RETURN ((LM_ERROR, "Opening Acceptor\n"),-1);

    ACE_DEBUG ((LM_DEBUG,"(%P|%t) starting up server logging daemon\n"));

    while (1)
        g_reactor->handle_events ();

    return 0;
}
