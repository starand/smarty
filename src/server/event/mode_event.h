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

class mode_event_t : public smarty::event_t
{
    friend class event_handler_t;

public:
    mode_event_t( uint mode_id, bool enabled,
                  smarty::command_handler_t& command_handler,
                  const uint& modes_bitset );
    ~mode_event_t( );

public: // event_intf_t
    virtual void on_event( );
    virtual uint get_mode( ) const;
    virtual void set_actions( std::vector< command_ptr_t >& actions );

protected:
    uint m_mode_id;
    bool m_is_enabled;

    const uint& m_modes_bitset;
    uint m_prev_bitset;

    std::vector< command_ptr_t > m_actions;
    smarty::command_handler_t& m_command_handler;
};

