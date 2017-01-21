#pragma once

#include <client/client_handler_factory.h>
#include <client/client_queue.h>
#include <client/client_register.h>

#include <threading/threading.h>
#include <memory>


class client_handler_intf_t;
class client_thread_t;
class desktop_register_t;
class device_t;
class event_handler_t;
class mobile_register_t;
class smarty_config_t;
class socket_t;
struct desktop_command_request_t;
struct mobile_notification_t;

namespace smarty
{
    class client_handler_factory_t;
}

class net_server_t
    : public thread_base_t
    , public smarty::client_register_t
    , public smarty::client_handler_factory_t
{
public:
    net_server_t( const smarty_config_t& config, device_t& device, event_handler_t& handler );
    ~net_server_t();

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

public:
    bool start_client_handlers( );
    void stop_client_handlers( );

    bool start_mobile_register( );
    void stop_mobile_register( );

    bool start_desktop_register( );
    void stop_desktop_register( );

public: // client_register_t
    virtual void on_notify_mobile_clients( const mobile_notification_t& notification );
    virtual void on_execute_desktop_command( const desktop_command_request_t& command );
    virtual void on_update_modes_request( uint modes_bitset );

public: // client_handler_factory_t methods
    virtual smarty::client_handler_t*
    create_mobile_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_req );

    virtual smarty::client_handler_t*
    create_desktop_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_req );

private:
    bool init( );
    void finalize( );

    void clean_clients_queue( );

private:
    const smarty_config_t& m_config;
    device_t& m_device;
    event_handler_t& m_event_handler;

    std::unique_ptr< socket_t > m_socket;
    ushort m_port;

    clients_queue_t m_clients_queue;
    std::vector< std::shared_ptr< client_thread_t > >  m_client_handlers;

    std::unique_ptr< mobile_register_t > m_mobile_register;
    std::unique_ptr< desktop_register_t > m_desktop_register;
};
