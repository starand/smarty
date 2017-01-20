#pragma once


struct mobile_notification_t;
struct desktop_command_request_t;

namespace smarty
{

//--------------------------------------------------------------------------------------------------

class client_linker_t
{
public:
    virtual ~client_linker_t( ) { }

    virtual void on_notify_mobile_clients( const mobile_notification_t& notification ) = 0;
    virtual void on_execute_desktop_command( const desktop_command_request_t& command ) = 0;

    virtual void on_update_modes_request( uint modes_bitset ) = 0;
};

//--------------------------------------------------------------------------------------------------

} // namespace smarty
