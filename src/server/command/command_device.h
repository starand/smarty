#pragma once

#include <command/command.h>

#include <common/driver_intf.h>


struct device_command_t;
class light_object_t;

class command_device_t : public smarty::command_t
{
public:
    command_device_t( const device_command_t& cmd, uint timeout, light_object_t& light_object );

public:
    virtual ErrorCode execute( driver_intf_t& driver );

private:
    device_command_t m_cmd;
    uint m_timeout;
    light_object_t& m_light_object;
};
