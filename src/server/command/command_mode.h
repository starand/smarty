#pragma once

#include <command/command.h>


class event_handler_t;

class command_mode_t : public smarty::command_t
{
public:
    command_mode_t( uint mode_bit, bool onOff, event_handler_t& event_handler, uint delay );

public:
    virtual ErrorCode execute( device_t& device );

private:
    event_handler_t& m_event_handler;

    uint m_mode_bit;
    bool m_onOff;
    uint m_delay;
};

