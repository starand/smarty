#pragma once

#include <threading/threading.h>
#include <memory>


class clients_queue_t;
class socket_t;

class net_server_t : public thread_base_t
{
public:
    net_server_t( clients_queue_t& clients_queue );

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

private:
    bool init( );
    void finalize( );

private:
    std::unique_ptr< socket_t > m_socket;
    ushort m_port;

    clients_queue_t& m_clients_queue;
};
