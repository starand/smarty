#pragma once

#include <common/types.h>

/**
 * EC_STATUS = EC__MIN, -- get device status
 * EC_TURNON            -- turn on one entity
 * EC_TURNOFF           -- turn off one entity
 * EC_TURNALL           -- turn off all entities
 * EC_TURNEQUAL         -- reserved
 * EC_TURNOFFBUTTON     -- disable button
 * EC_TURNONBUTTON      -- enable button
 */
typedef uchar device_cmd_t;

typedef uint pin_state_t;
typedef pin_state_t device_param_t;

struct device_command_t
{
    device_cmd_t cmd;
    device_param_t param;
};

typedef pin_state_t lights_state_t;
typedef pin_state_t buttons_state_t;
typedef pin_state_t sensors_state_t;

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


class driver_t
{
public:
    virtual bool execute_command( const device_command_t& command, device_state_t& state ) = 0;
};
