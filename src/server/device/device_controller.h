#ifndef __STATE_CHECKER_H_INCLUDED
#define __STATE_CHECKER_H_INCLUDED

#include <common/driver_intf.h>
#include <common/errors.h>

#include <threading.h>


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
    virtual void notify( const device_state_t& state );

public:
    lights_state_t get_lights_state( );
    void wait_state_update( ) const;

    ErrorCode execute_command( device_cmd_t command, device_param_t param );
    ErrorCode execute_command( const device_command_t& cmd );

private:
    void check_for_changes( );
    void check_lights_changes( );
    void check_buttons_changes( );
    void check_sensors_changes( );

private:
    driver_intf_t& m_driver;
    smarty_config_t& m_config;

    device_state_t m_device_state;
    event_t m_update_event;
    device_state_t m_prev_device_state;
};

#endif // __STATE_CHECKER_H_INCLUDED
