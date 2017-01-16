#ifndef __STATUS_UPDATER_H_INCLUDED
#define __STATUS_UPDATER_H_INCLUDED

#include <common/driver_intf.h>

#include <client/client_heartbeat.h>
#include <client/client_info.h>


class config_t;
struct desktop_command_request_t;
struct mobile_notification_t;
class socket_t;

namespace smarty
{
    class client_linker_t;
}


class mobile_register_t : public client_heartbeat_t, public device_observer_t
{
public:
    mobile_register_t( smarty::client_linker_t& desktop_connector, const config_t& config,
                       const device_state_t& state );
    ~mobile_register_t( );

protected: // client_heartbeat_t
    virtual void check_liveness( );

public: // device_observer_t
    virtual void on_light_changed( const lights_state_t& state );
    virtual void on_button_pressed( const buttons_state_t& state );
    virtual void on_sensor_triggered( const sensors_state_t& state );

public:
    bool update_config( socket_t& client );

    void add_client( socket_t *client, const string& end_point );
    void remove_client( socket_t *client );

    void notify_clients( const mobile_notification_t& notification );
    void notify_modes_update( uint modes );

    void set_heartbeat_lasttime( socket_t *socket );

private:
    void update_clients( const lights_state_t& state );

    void execute_desktop_command( const desktop_command_request_t& command );

    void on_client_connected( );
    client_list_t::iterator find_mobile( socket_t *socket );
    void drop_client( client_list_t::iterator& itList );

private:
    smarty::client_linker_t& m_desktop_connector;

    client_list_t m_client_list;
    mutex_t m_client_list_mutex;

    device_state_t m_prev_state;

    const config_t& m_config;
};

#endif // __STATUS_UPDATER_H_INCLUDED
