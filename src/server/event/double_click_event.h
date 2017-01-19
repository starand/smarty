#pragma once

#include <command/command_handler.h>
#include <event/event.h>


namespace smarty
{
    class command_handler_t;
    class command_t;
}


class double_click_event_t : public smarty::event_t
{
public:
    double_click_event_t( uint pin, uint mode, const uint& last_dblclick,
                          smarty::command_handler_t& command_handler );
    ~double_click_event_t( );

public: // event_intf_t
    virtual void on_event( );
    virtual uint get_mode( ) const;
    virtual void set_actions( std::vector< command_ptr_t >& actions );

private:
    uint m_pin;
    uint m_mode;

    const uint& m_last_dblclicked;
    std::vector< command_ptr_t > m_actions;
    smarty::command_handler_t& m_command_handler;
};
