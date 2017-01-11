#pragma once

#include <event/event_factory.h>
#include <common/driver_intf.h>

#include <memory>


class config_t;
class command_processor_t;
struct device_state_t;
class event_parser_t;
class light_object_t;

typedef std::vector< std::shared_ptr< smarty::event_t > > events_t;

class event_handler_t : public device_observer_t, public smarty::event_factory_t
{
public:
    event_handler_t( const config_t& config, command_processor_t& command_handler,
                     const device_state_t& state );
    ~event_handler_t( );

public:
    bool init( );

    void update_modes( uint bitset );
    uint get_modes_bitset( ) const;

public: // device_observer_t
    virtual void notify( const device_state_t& state );

public: // event_factory_intf_t
    virtual std::shared_ptr< smarty::event_t >
    create_sensor_event( uint pin, TriggerState state, uint mode );

    virtual std::shared_ptr< smarty::event_t >
    create_button_event( uint pin, TriggerState state, uint mode );

    virtual std::shared_ptr< smarty::event_t >
    create_light_event( uint pin, TriggerState state, uint mode );

    std::shared_ptr< smarty::event_t >
    create_mode_event( uint mode, bool enabled );

private:
    const config_t& m_config;
    std::unique_ptr< event_parser_t > m_event_parser;

    command_processor_t& m_command_handler;
    device_state_t m_device_state;

    events_t m_device_events;
    events_t m_mode_events;

    uint m_event_modes_bitset;
};
