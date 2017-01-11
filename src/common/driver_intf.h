#ifndef __CONTROLLER_INTF_H_INCLUDED
#define __CONTROLLER_INTF_H_INCLUDED

#include <common/errors.h>


#define COMMAND_LENGTH	6
#define LIGHTS_BYTE		3
#define BUTTONS_BYTE	4
#define IR_BYTE			5
#define SENSORS_BYTE	2


//--------------------------------------------------------------------------------------------------

// state type definitions
typedef uchar lights_state_t;
typedef uchar buttons_state_t;
typedef uchar sensors_state_t;

struct device_state_t
{
    lights_state_t lights;
    buttons_state_t buttons;
    sensors_state_t sensors;

    bool operator!=( const device_state_t& rhs ) const
    {
        return lights != rhs.lights || buttons != rhs.buttons || sensors != rhs.sensors;
    }
};

//--------------------------------------------------------------------------------------------------

// command type definitions
typedef uchar device_cmd_t;
typedef uchar device_param_t;

struct device_command_t
{
    device_cmd_t cmd;
    device_param_t param;
};

//--------------------------------------------------------------------------------------------------

class device_observer_t
{
public:
    virtual ~device_observer_t( ) { }

    virtual void notify( const device_state_t& state ) = 0; // this method should be fast
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
