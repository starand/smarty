#ifndef __CONTROLLER_INTF_H_INCLUDED
#define __CONTROLLER_INTF_H_INCLUDED

#include <driver/driver.h>
#include <common/errors.h>


class device_observer_t
{
public:
    virtual ~device_observer_t( ) { }

    virtual void on_light_changed( const lights_state_t& bitset ) = 0;
    virtual void on_button_pressed( const buttons_state_t& bitset ) = 0;
    virtual void on_sensor_triggered( const sensors_state_t& bitset ) = 0;

    virtual void on_double_click( uint button_pin ) = 0;
};


class driver_intf_t
{
public:
    virtual ~driver_intf_t( ) { }

    virtual ErrorCode execute_command( const device_command_t& command ) = 0;
    virtual ErrorCode get_state( device_state_t& state ) const = 0;

    virtual ErrorCode add_observer( device_observer_t& observer ) = 0;
    virtual ErrorCode remove_observer( device_observer_t& observer ) = 0;
};

//--------------------------------------------------------------------------------------------------

typedef driver_intf_t* ( *create_driver_funct_t )( );
typedef void( *free_driver_funct_t )( );

//--------------------------------------------------------------------------------------------------

#endif // __CONTROLLER_INTF_H_INCLUDED
