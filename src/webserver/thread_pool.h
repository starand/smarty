#ifndef __THREAD_POOL_H_INCLUDED
#define __THREAD_POOL_H_INCLUDED

#include <memory>
#include <vector>


class clients_queue_t;
class webclient_handler_t;
class driver_intf_t;
class smarty_config_t;
class request_executor_t;

class thread_pool_t
{
public:
    thread_pool_t( clients_queue_t& clients_queue, std::shared_ptr< driver_intf_t > driver,
                   std::shared_ptr< smarty_config_t > config );
    ~thread_pool_t( );

private:
    void create_internal_objects( );
    void destroy_internal_obnjects( );

public:
    bool start( size_t pool_size );
    bool stop( );

private:
    typedef std::vector< webclient_handler_t* > handlers_collection_t;

private:
    clients_queue_t& m_clients_queue;

    size_t m_pool_size;
    handlers_collection_t m_threads;

    std::shared_ptr< driver_intf_t > m_driver;
    std::shared_ptr< smarty_config_t > m_config;

    request_executor_t *m_request_executor;
};

#endif // __THREAD_POOL_H_INCLUDED
