#pragma once

#include <common/types.h>

typedef uchar device_cmd_t;
typedef uchar device_param_t;

struct device_command_t
{
    device_cmd_t cmd;
    device_param_t param;
};

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


class driver_t
{
public:
    virtual bool execute_command( const device_command_t& command, device_state_t& state ) = 0;
};
