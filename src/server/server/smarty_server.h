#pragma once

#include <common/server_intf.h>

#include <memory>


class driver_intf_t;
class device_t;
class event_handler_t;
class net_server_t;
class smarty_config_t;

class smarty_server_t : public server_intf_t
{
public:
    smarty_server_t( );
    ~smarty_server_t( );

private:
    bool start_device( std::shared_ptr< driver_intf_t > driver );
    void stop_device( );

    bool start_event_handler( );
    void stop_event_handler( );

    bool start_net_server( );
    void stop_net_server( );

public: // server_intf_t implementation
    virtual ErrorCode start( std::shared_ptr< driver_intf_t > driver,
                             std::shared_ptr< smarty_config_t > config );
    virtual ErrorCode stop( );

private:
    std::shared_ptr< smarty_config_t > m_config;

    std::unique_ptr< device_t > m_device;

    std::shared_ptr< net_server_t > m_net_server;
    std::unique_ptr< event_handler_t > m_event_handler;
};
