#include <common/StdAfx.h>

#include <command/command_device.h>
#include <command/command_processor.h>
#include <device/light_object.h>
#include <event/event_handler.h>
#include <event/event_parser.h>
#include <event/mode_event.h>
#include <event/device_event.h>

#include <common/driver_intf.h>
#include <files/config.h>
#include <utils/utils.h>

#include <memory>


//--------------------------------------------------------------------------------------------------

event_handler_t::event_handler_t( const config_t& config, command_processor_t& command_handler,
                                  const device_state_t& state )
    : m_config( config )
    , m_event_parser( new event_parser_t( *this, command_handler ) )
    , m_command_handler( command_handler )
    , m_device_state( state )
    , m_light_events( )
    , m_button_events( )
    , m_sensor_events( )
    , m_mode_events( )
    , m_event_modes_bitset( 0 )
{
}

//--------------------------------------------------------------------------------------------------

event_handler_t::~event_handler_t( )
{
}

//--------------------------------------------------------------------------------------------------

bool event_handler_t::init( )
{
    if ( !m_event_parser->parse( m_config ) )
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void event_handler_t::update_modes( uint bitset )
{
    m_event_modes_bitset = bitset;
    LOG_TRACE( "[event.modes] update modes with value %u", bitset );

    for ( auto& event_handler : m_mode_events )
    {
        event_handler->on_event( );
    }
}

//--------------------------------------------------------------------------------------------------

uint event_handler_t::get_modes_bitset( ) const
{
    return m_event_modes_bitset;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::on_light_changed( const lights_state_t& state )
{
    m_device_state.lights = state;
    for ( auto& handler : m_light_events )
    {
        if ( check_mode( *handler ) )
        {
            handler->on_event( );
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::on_button_pressed( const buttons_state_t& state )
{
    m_device_state.buttons = state;
    for ( auto& handler : m_button_events )
    {
        if ( check_mode( *handler ) )
        {
            handler->on_event( );
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::on_sensor_triggered( const sensors_state_t& state )
{
    m_device_state.sensors = state;
    for ( auto& handler : m_sensor_events )
    {
        if ( check_mode( *handler ) )
        {
            handler->on_event( );
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::on_double_click( uint button_pin )
{
    // TODO: implement on double click
}

//--------------------------------------------------------------------------------------------------

/*virtual */
std::shared_ptr< smarty::event_t >
event_handler_t::create_device_event( DeviceEventType type,
                                      uint pin, TriggerState state, uint mode )
{
    auto event = std::make_shared< device_event_t >( type, pin, state, mode,
                                                     m_command_handler, m_device_state );
    switch ( type )
    {
    case DeviceEventType::LIGHT:
        m_light_events.emplace_back( event );
        LOG_TRACE( "[event.light] on state %u, pin #%u created", state, pin );
        break;
    case DeviceEventType::BUTTON:
        m_button_events.emplace_back( event );
        LOG_TRACE( "[event.button] on state %u, pin #%u created", state, pin );
        break;
    case DeviceEventType::SENSOR:
        m_sensor_events.emplace_back( event );
        LOG_TRACE( "[event.sensor] on state %u, pin #%u created", state, pin );
        break;
    default:
        ASSERT( false && "Unsupported device event" );
    }

    return event;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
std::shared_ptr< smarty::event_t >
event_handler_t::create_mode_event( uint mode, bool enabled )
{
    auto event = std::make_shared< mode_event_t >( mode, enabled,
                                                   m_command_handler, m_event_modes_bitset );
    m_mode_events.emplace_back( event );

    LOG_TRACE( "[event.mode] on mode %u, changed to \'%s\'", mode, ( enabled ? "on" : "off" ) );
    return event;
}

//--------------------------------------------------------------------------------------------------

bool event_handler_t::check_mode( const smarty::event_t& handler ) const
{
    uint mode = handler.get_mode( );
    return ( m_event_modes_bitset & mode ) != 0 || mode == 0;
}

//--------------------------------------------------------------------------------------------------
