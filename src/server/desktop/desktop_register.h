#ifndef __DESKTOP_REGISTER_H_INCLUDED
#define __DESKTOP_REGISTER_H_INCLUDED

#include <client/client_heartbeat.h>
#include <desktop/desktop_info_list.h>

#include <map>


class socket_t;
struct desktop_command_request_t;
struct mobile_notification_t;

namespace smarty
{
    class client_register_t;
}


class desktop_register_t : public client_heartbeat_t
{
public:
    desktop_register_t( smarty::client_register_t& clients );
    ~desktop_register_t( );

protected: // client_heartbeat_t
    virtual void check_liveness( );

public:
    void add_desktop( socket_t *socket, uint desktop_index, const string& name );

    void remove_desktop( uint index );
    void remove_desktop( socket_t *socket );

    void set_heartbeat_lasttime( uint index );

private:
    void do_remove_desktop( uint index );

    void send_notification( const mobile_notification_t& notification );

    void on_desktop_connected( uint desktop_index, const string& name );
    void on_desktop_disconnected( uint index );

public:
    void execute_command( const desktop_command_request_t& command );

private:
    void execute_desktop_command( const desktop_command_request_t& command );
    void refresh_desktops_info( );

private:
    desktop_info_list_t m_desktops;
    mutex_t m_clients_lock;

    smarty::client_register_t& m_clients;
};

#endif // __DESKTOP_REGISTER_H_INCLUDED
