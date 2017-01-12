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
    , m_device_events( )
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
void event_handler_t::notify( const device_state_t& state )
{
    m_device_state = state;
    for ( auto& handler : m_device_events )
    {
        bool mode_enabled = ( m_event_modes_bitset & handler->get_mode( ) ) != 0;

        // if mode is 'all' or is enabled
        if ( handler->get_mode( ) == 0 || mode_enabled )
        {
            handler->on_event( );
        }
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
std::shared_ptr< smarty::event_t >
event_handler_t::create_device_event( DeviceEventType type,
                                      uint pin, TriggerState state, uint mode )
{
    auto event = std::make_shared< device_event_t >( type, pin, state, mode,
                                                     m_command_handler, m_device_state );
    m_device_events.emplace_back( event );

    LOG_TRACE( "[event.sensor] on state %u, pin #%u created", state, pin );
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
