#pragma once

#include <common/driver_intf.h>
#include <common/enums.h>
#include <common/types.h>
#include <event/event.h>

#include <memory>


namespace smarty
{
    class command_handler_t;
    class command_t;
}

class light_object_t;

class sensor_event_t : public smarty::event_t
{
    friend class event_handler_t;

public:
    sensor_event_t( uint pin, TriggerState trigger_state, uint mode,
                    smarty::command_handler_t& command_handler,
                    const device_state_t& device_state );
    ~sensor_event_t( );

public: // event_intf_t
    virtual void on_event( );
    virtual uint get_mode( );
    virtual void set_actions( std::vector< std::shared_ptr< smarty::command_t > >& actions );

protected:
    uint m_sensor_pin;
    TriggerState m_trigger_state;
    uint m_mode;

    const device_state_t& m_device_state;
    device_state_t m_prev_state;

    std::vector< std::shared_ptr< smarty::command_t > > m_actions;
    smarty::command_handler_t& m_command_handler;
};

