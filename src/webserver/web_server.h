#ifndef __WEB_SERVER_H_INCLUDED
#define __WEB_SERVER_H_INCLUDED

#include <common/webserver_intf.h>

#include <threading.h>
#include <lock_queue.h>


class config_t;
class socket_t;
class clients_queue_t : public lock_queue_t < socket_t * > { };
class thread_pool_t;


class web_server_t : public web_server_intf_t, public thread_base_t
{
public:
    web_server_t( );
    ~web_server_t( );

private:
    void create_internal_objects( std::shared_ptr< driver_intf_t > driver,
                                  std::shared_ptr< config_t > config );
    void destroy_internal_objects( );

public: // web_client_intf_t implementation
    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< config_t > config );
    virtual ErrorCode stop( );

public: // thread_base_t implementation
    virtual void do_run( );
    virtual void do_stop( );


private:
    socket_t *m_socket;

    clients_queue_t m_clients_queue;
    thread_pool_t *m_thread_pool;
};

#endif // __WEB_SERVER_H_INCLUDED
