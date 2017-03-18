#pragma once

#include <command/command_handler.h>
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

class device_event_t : public smarty::event_t
{
    friend class event_handler_t;

public:
    device_event_t( DeviceEventType type, uint pin, TriggerState trigger_state, uint mode,
                    smarty::command_handler_t& command_handler,
                    const device_state_t& device_state );

public: // event_intf_t
    virtual void on_event( );

protected:
    DeviceEventType m_type;
    uint m_pin;
    TriggerState m_trigger_state;

    const device_state_t& m_device_state;
    device_state_t m_prev_state;

    smarty::command_handler_t& m_command_handler;
};

