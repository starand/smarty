#pragma once

#include <client/client_queue.h>

#include <threading/threading.h>
#include <memory>


class client_thread_t;
class socket_t;

namespace smarty
{
    class client_handler_factory_t;
}


class net_server_t : public thread_base_t
{
public:
    net_server_t( smarty::client_handler_factory_t& handler_factory );

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

public:
    bool start_client_handlers( );
    void stop_client_handlers( );

private:
    bool init( );
    void finalize( );

    void clean_clients_queue( );

private:
    smarty::client_handler_factory_t& m_handler_factory;

    std::unique_ptr< socket_t > m_socket;
    ushort m_port;

    clients_queue_t m_clients_queue;
    std::vector< std::shared_ptr< client_thread_t > >  m_client_handlers;
};
