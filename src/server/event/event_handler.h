#pragma once

#include <command/command_factory.h>
#include <command/command_handler.h>
#include <event/event_factory.h>

#include <common/driver_intf.h>

#include <lock_queue.h>
#include <threading/threading.h>

#include <memory>


class config_t;
struct device_state_t;
class event_parser_t;
class light_object_t;

namespace smarty
{
    class command_t;
}

typedef std::vector< std::shared_ptr< smarty::event_t > > events_t;

//--------------------------------------------------------------------------------------------------

class event_handler_t
    : public device_observer_t
    , public smarty::event_factory_t
    , public thread_base_t
    , public smarty::command_handler_t
    , public smarty::command_factory_t
{
public:
    event_handler_t( const config_t& config, driver_intf_t& driver, const device_state_t& state );
    ~event_handler_t( );

public:
    bool init( );

    void update_modes( uint bitset );
    uint get_modes_bitset( ) const;

public: // device_observer_t
    virtual void on_light_changed( const lights_state_t& state );
    virtual void on_button_pressed( const buttons_state_t& state );
    virtual void on_sensor_triggered( const sensors_state_t& state );
    virtual void on_double_click( uint button_pin );

public: // event_factory_intf_t
    virtual std::shared_ptr< smarty::event_t >
    create_device_event( DeviceEventType type, uint pin, TriggerState state, uint mode );

    std::shared_ptr< smarty::event_t >
    create_mode_event( uint mode, bool enabled );

public: // thread_base_t
    virtual void do_run( );
    virtual void do_stop( );

public: // smarty::command_handler_t
    void add_command( std::shared_ptr< smarty::command_t > cmd );

public: // smarty::command_factory_t
    virtual std::shared_ptr< smarty::command_t >
    create_device_command( const device_command_t& cmd, uint timeout );

private:
    bool check_mode( const smarty::event_t& handler ) const;
    void process_command( );

    bool init_light_objects( );
    void check_light_objects( );

private:
    const config_t& m_config;
    driver_intf_t& m_driver;

    std::unique_ptr< event_parser_t > m_event_parser;

    device_state_t m_device_state;
    uint m_last_dblclck_pin;

    events_t m_light_events;
    events_t m_button_events;
    events_t m_sensor_events;
    events_t m_mode_events;
    events_t m_double_click_events;

    uint m_event_modes_bitset;

    lock_queue_t< std::shared_ptr< smarty::command_t > > m_cmd_queue;
    std::vector< light_object_t > m_lights;
};

//--------------------------------------------------------------------------------------------------
