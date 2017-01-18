#ifndef __TCP_SERVER_H_INCLUDED
#define __TCP_SERVER_H_INCLUDED

#include <client/client_linker.h>
#include <client/client_handler_factory.h>
#include <client/client_queue.h>

#include <common/server_intf.h>

#include <memory>
#include <set>


class client_thread_t;
class client_handler_intf_t;
class driver_intf_t;
class desktop_register_t;
class device_t;
struct device_state_t;
class event_handler_t;
class mobile_register_t;
class net_server_t;
class smarty_config_t;
class socket_t;
struct update_modes_request_t;


class smarty_server_t
    : public server_intf_t
    , public smarty::client_linker_t
    , public smarty::client_handler_factory_t
{
public:
    smarty_server_t( );
    ~smarty_server_t( );

private:
    bool start_device( const device_state_t& state );
    void stop_device( );

    bool start_mobile_register( const device_state_t& state );
    void stop_mobile_register( );

    bool start_desktop_register( );
    void stop_desktop_register( );

    bool start_client_handlers( );
    void stop_client_handlers( );

    bool start_event_handler( const device_state_t& state );
    void stop_event_handler( );

    bool start_net_server( );
    void stop_net_server( );

    void clean_clients_queue( );

public: // server_intf_t implementation
    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< smarty_config_t > config );
    virtual ErrorCode stop( );

public: // client_linker_t
    virtual void on_notify_mobile_clients( const mobile_notification_t& notification );
    virtual void on_execute_desktop_command( const desktop_command_request_t& command );
    virtual void on_update_modes_request( const update_modes_request_t& request );

public: // client_handler_factory_t methods
    virtual smarty::client_handler_t*
    create_mobile_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_req );

    virtual smarty::client_handler_t*
    create_desktop_handler( socket_t& socket, const char *endpoint, packet_intf_t *hs_req );

public:
    clients_queue_t& get_client_queue( ) const
    {
        return const_cast<clients_queue_t& >( m_clients_queue );
    }

    const event_handler_t& get_event_handler( ) const;

private:
    clients_queue_t m_clients_queue;
    std::unique_ptr< net_server_t > m_net_server;
    std::unique_ptr< device_t > m_device;

    std::shared_ptr< smarty_config_t > m_config;
    std::shared_ptr< driver_intf_t > m_driver;

    std::unique_ptr< event_handler_t > m_event_handler;

    client_thread_t **m_handlers;

    std::unique_ptr< mobile_register_t > m_mobile_register;
    std::unique_ptr< desktop_register_t > m_desktop_register;
};

#endif // __TCP_SERVER_H_INCLUDED
