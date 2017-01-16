#ifndef __STATE_CHECKER_H_INCLUDED
#define __STATE_CHECKER_H_INCLUDED

#include <common/driver_intf.h>
#include <common/errors.h>

#include <threading.h>

#include <memory>
#include <set>


class driver_intf_t;
class event_t;
class smarty_config_t;
class command_processor_t;


class device_controller_t : public thread_base_t, public device_observer_t
{
public:
    device_controller_t( driver_intf_t& driver, smarty_config_t& config,
                         const device_state_t& state );
    ~device_controller_t( );

private:
    void create_internal_objects( );
    void destroy_internal_objects( );

    void initialize( );

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
    uint m_double_click_pin;
    event_t m_update_event;
    device_state_t m_prev_device_state;

    std::set< device_observer_t* > m_observers;
};

#endif // __STATE_CHECKER_H_INCLUDED
