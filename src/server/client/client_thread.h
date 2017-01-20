#ifndef __CLIENT_HANDLER_H_INCLUDED
#define __CLIENT_HANDLER_H_INCLUDED

#include <threading.h>


class clients_queue_t;
class socket_t;

namespace smarty
{
    class client_handler_t;
}


namespace smarty
{
    class client_handler_factory_t;
}


class client_thread_t : public thread_base_t
{
public:
    client_thread_t( smarty::client_handler_factory_t& clients_factory,
                     clients_queue_t& clients_queue );
    ~client_thread_t( );

public:
    virtual void do_run( );
    virtual void do_stop( );

private:
    smarty::client_handler_t *create_client_handler( );
    void finish_connection( );

private:
    string m_end_point;
    socket_t *m_socket;

    clients_queue_t& m_clients_queue;
    smarty::client_handler_factory_t& m_clients_factory;
};

#endif // __CLIENT_HANDLER_H_INCLUDED
