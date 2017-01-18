#include <common/StdAfx.h>

#include <command/command_device.h>
#include <device/light_object.h>
#include <event/device_event.h>
#include <event/double_click_event.h>
#include <event/event_handler.h>
#include <event/event_parser.h>
#include <event/mode_event.h>

#include <common/driver_intf.h>
//#include <common/errors.h>
//#include <common/mc_enum.h>
#include <files/config.h>
#include <utils/utils.h>

#include <memory>


#define INVALID_PIN (uint)-1


namespace
{

//--------------------------------------------------------------------------------------------------

uint get_bit_offset( device_param_t param )
{
    uint result = 0;
    while ( param && ( param & 1 ) == 0 )
    {
        ++result;
        param >>= 1;
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------

event_handler_t::event_handler_t( const config_t& config, driver_intf_t& driver,
                                  const device_state_t& state  )
    : m_config( config )
    , m_driver( driver )
    , m_event_parser( new event_parser_t( *this, *this ) )
    , m_device_state( state )
    , m_last_dblclck_pin( INVALID_PIN )
    , m_light_events( )
    , m_button_events( )
    , m_sensor_events( )
    , m_mode_events( )
    , m_double_click_events( )
    , m_event_modes_bitset( 0 )
    , m_cmd_queue( )
    , m_lights( )
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

    if ( !init_light_objects( ) )
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_handler_t::init_light_objects( )
{
    auto lights_node = m_config[ "lights" ];
    if ( lights_node.isNull( ) )
    {
        LOG_ERROR( "Lights node not set in config" );
        return false;
    }

    uint lights_count = lights_node.size( );
    ASSERT( lights_count != 0 );
    m_lights.resize( lights_count );

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

/*virtual */ // provides number of button pin starting from 0
void event_handler_t::on_double_click( uint button_pin )
{
    m_last_dblclck_pin = button_pin;
    for ( auto& handler : m_double_click_events )
    {
        if ( check_mode( *handler ) )
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
    std::shared_ptr< smarty::event_t > event;

    switch ( type )
    {
    case DeviceEventType::LIGHT:
        event = std::make_shared< device_event_t >(
                        type, pin, state, mode, *this, m_device_state );
        m_light_events.emplace_back( event );
        LOG_TRACE( "[event.light] on state %u, pin #%u created", state, pin );
        break;
    case DeviceEventType::BUTTON:
        event = std::make_shared< device_event_t >(
                        type, pin, state, mode, *this, m_device_state );
        m_button_events.emplace_back( event );
        LOG_TRACE( "[event.button] on state %u, pin #%u created", state, pin );
        break;
    case DeviceEventType::SENSOR:
        event = std::make_shared< device_event_t >(
                        type, pin, state, mode, *this, m_device_state );
        m_sensor_events.emplace_back( event );
        LOG_TRACE( "[event.sensor] on state %u, pin #%u created", state, pin );
        break;
    case DeviceEventType::DOUBLE_CLICK:
        event = std::make_shared< double_click_event_t >(
                        pin, mode, m_last_dblclck_pin, *this );
        m_double_click_events.emplace_back( event );
        LOG_TRACE( "[event.dblclck] for pin #%u created", state, pin );
        break;
    default:
        ASSERT( false && "Unsupported device event" );
    }

    ASSERT( event != nullptr && "event should be null" );
    return event;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
std::shared_ptr< smarty::event_t >
event_handler_t::create_mode_event( uint mode, bool enabled )
{
    auto event = std::make_shared< mode_event_t >( mode, enabled, *this, m_event_modes_bitset );
    m_mode_events.emplace_back( event );

    LOG_TRACE( "[event.mode] on mode %u, changed to \'%s\'", mode, ( enabled ? "on" : "off" ) );
    return event;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::do_run( )
{
    while ( true )
    {
        sleep( 100 );
        if ( is_stopping( ) )
        {
            break;
        }

        if ( !m_cmd_queue.empty( ) )
        {
            process_command( );
        }

        check_light_objects( );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void event_handler_t::do_stop( )
{
    class fake_command : public smarty::command_t
    {
    public:
        virtual ErrorCode execute( driver_intf_t& driver ) { return ErrorCode::OK; }
    };

    std::shared_ptr< smarty::command_t > fake( new fake_command( ) );
    m_cmd_queue.push( fake );
}

//--------------------------------------------------------------------------------------------------

void event_handler_t::add_command( std::shared_ptr< smarty::command_t > cmd )
{
    m_cmd_queue.push( cmd );
    LOG_TRACE( "[cmd.%p] Command added to event handler", cmd.get( ) );
}

//--------------------------------------------------------------------------------------------------

std::shared_ptr< smarty::command_t >
event_handler_t::create_device_command( const device_command_t& cmd, uint timeout )
{
    uint idx = get_bit_offset( cmd.param );
    auto res = std::make_shared< command_device_t >( cmd, timeout, m_lights[ idx ] );
    LOG_DEBUG( "[cmd.%p] Device command [%u:%u] with params (timeout %u) created",
               res.get(), cmd.cmd, cmd.param, timeout );

    return res;
}

//--------------------------------------------------------------------------------------------------

bool event_handler_t::check_mode( const smarty::event_t& handler ) const
{
    uint mode = handler.get_mode( );
    return ( m_event_modes_bitset & mode ) != 0 || mode == 0;
}

//--------------------------------------------------------------------------------------------------

void event_handler_t::process_command( )
{
    auto command = m_cmd_queue.pop( );
    ErrorCode code = command->execute( m_driver );

    if ( code != ErrorCode::OK )
    {
        LOG_ERROR( "[cmd.%p] Command failed with code %u", command.get( ), code );
    }

    LOG_TRACE( "[cmd.%p] Command processed", command.get( ) );
}

//--------------------------------------------------------------------------------------------------

void event_handler_t::check_light_objects( )
{
    time_t current = time( nullptr );

    uint count = m_lights.size( );
    for ( uint idx = 0; idx < count; ++idx )
    {
        auto& light = m_lights[ idx ];

        time_t timeout = light.get_turnoff_timeout( );
        if ( timeout && timeout <= current )
        {
            device_command_t cmd{ EC_TURNOFF, static_cast<device_param_t>( 1 << idx ) };
            add_command( std::make_shared< command_device_t >( cmd, 0, light ) );

            light.clear_turnoff_time( );
        }
    }
}

//--------------------------------------------------------------------------------------------------
