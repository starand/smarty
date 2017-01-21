#pragma once

#include <command/command.h>


class event_handler_t;

struct mode_cmd_t
{
    uint mode_bit;
    bool onOff;
};

class command_mode_t : public smarty::command_t
{
public:
    command_mode_t( mode_cmd_t cmd, uint delay, event_handler_t& event_handler );

public:
    virtual ErrorCode execute( );

private:
    mode_cmd_t m_cmd;
    uint m_delay;

    event_handler_t& m_event_handler;
};

