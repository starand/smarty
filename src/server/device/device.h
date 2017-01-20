#pragma once

#include <common/driver_intf.h>
#include <common/errors.h>

#include <threading.h>

#include <memory>
#include <set>


class driver_intf_t;
class event_t;
class smarty_config_t;


class device_t : public thread_base_t, public device_observer_t
{
public:
    device_t( driver_intf_t& driver, smarty_config_t& config );
    ~device_t( );

public:
    bool init( );

public: // thread_base_t methods
    virtual void do_run( );
    virtual void do_stop( );

public: // device_observer_t methods
    virtual void on_light_changed( const lights_state_t& state );
    virtual void on_button_pressed( const buttons_state_t& state );
    virtual void on_sensor_triggered( const sensors_state_t& state );
    virtual void on_double_click( uint button_pin );

public:
    lights_state_t get_lights_state( );
    device_state_t get_device_state( ) const;
    void wait_state_update( ) const;

    ErrorCode execute_command( device_cmd_t command, device_param_t param );
    ErrorCode execute_command( const device_command_t& cmd );

    void add_observer( device_observer_t& observer );
    void remove_observer( device_observer_t& observer );

private:
    void check_for_changes( );
    void check_lights_changes( );
    void check_buttons_changes( );
    void check_sensors_changes( );
    void check_double_click( );

private:
    driver_intf_t& m_driver;
    smarty_config_t& m_config;

    device_state_t m_device_state;
    device_state_t m_prev_device_state;
    uint m_double_click_pin;

    event_t m_update_event;
    std::set< device_observer_t* > m_observers;
};
