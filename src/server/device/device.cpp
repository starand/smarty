#include <common/StdAfx.h>

#include <command/command_device.h>
#include <device/device.h>

#include <common/smarty_config.h>
#include <common/config_options.h>
#include <common/driver_intf.h>
#include <common/enums.h>

#include <utils.h>


#define PIN_NOT_SET (uint)-1


// device_t implementation
//--------------------------------------------------------------------------------------------------

device_t::device_t( driver_intf_t& driver, smarty_config_t& config )
    : m_driver( driver )
    , m_config( config )
    , m_device_state( )
    , m_double_click_pin( PIN_NOT_SET )
    , m_update_event( true, false )
    , m_prev_device_state( )
    , m_observers( )
{
    m_driver.get_state( m_device_state );
    m_prev_device_state = m_device_state;
}

//--------------------------------------------------------------------------------------------------

device_t::~device_t( )
{
}

//--------------------------------------------------------------------------------------------------

bool device_t::init( )
{
    // to get real state we have to execute command
    device_command_t status_command = { EC_STATUS, 0x00 };
    m_driver.execute_command( status_command );

    device_state_t state;
    if ( m_driver.get_state( state ) != ErrorCode::OK )
    {
        LOG_ERROR( "Unable to retreive device state" );
        return false;
    }

    auto off_on_start_node = m_config[ DEVICE_SECTION ][ DEVICE_OFF_ON_START ];
    ushort off_on_start = off_on_start_node.isUInt( ) ? off_on_start_node.asUInt( ) : 0;

    if ( off_on_start != 0 )
    {
        device_command_t command = { EC_TURNALL, 0x00 };
        m_driver.execute_command( command ); // turn all off lights on start
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_t::do_run( )
{
    m_driver.add_observer( *this );

    while ( true )
    {
        wait_state_update( );

        if ( is_stopping( ) )
        {
            break;
        }

        check_for_changes( );
        m_update_event.reset( );
    }

    m_driver.remove_observer( *this );
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_t::do_stop( )
{
    m_update_event.set( ); // to stop clients status updater
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_t::on_light_changed( const lights_state_t& state )
{
    m_device_state.lights = state;
    m_update_event.set( ); // notify about changes
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_t::on_button_pressed( const buttons_state_t& state )
{
    m_device_state.buttons = state;
    m_update_event.set( ); // notify about changes
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_t::on_sensor_triggered( const sensors_state_t& state )
{
    m_device_state.sensors = state;
    m_update_event.set( ); // notify about changes
}

/*virtual */
void device_t::on_double_click( uint button_pin )
{
    m_double_click_pin = button_pin;
    m_update_event.set( ); // notify about changes
}

//--------------------------------------------------------------------------------------------------

lights_state_t device_t::get_lights_state( )
{
    return m_device_state.lights;
}

//--------------------------------------------------------------------------------------------------

void device_t::wait_state_update( ) const
{
    m_update_event.wait( );
}

//--------------------------------------------------------------------------------------------------

ErrorCode device_t::execute_command( device_cmd_t command, device_param_t param )
{
    return m_driver.execute_command( { command, param } );
}

//--------------------------------------------------------------------------------------------------

ErrorCode device_t::execute_command( const device_command_t& cmd )
{
    return m_driver.execute_command( cmd );
}

//--------------------------------------------------------------------------------------------------

void device_t::add_observer( device_observer_t& observer )
{
    m_observers.insert( &observer );
}

//--------------------------------------------------------------------------------------------------

void device_t::remove_observer( device_observer_t& observer )
{
    m_observers.erase( &observer );
}

//--------------------------------------------------------------------------------------------------

void device_t::check_for_changes( )
{
    check_lights_changes( );
    check_buttons_changes( );
    check_sensors_changes( );
    check_double_click( );

    m_prev_device_state = m_device_state;
    m_double_click_pin = PIN_NOT_SET;
}

//--------------------------------------------------------------------------------------------------

void device_t::check_buttons_changes( )
{
    buttons_state_t& current_state = m_device_state.buttons;
    buttons_state_t& prev_state = m_prev_device_state.buttons;

    if ( prev_state != current_state )
    {
        for ( auto& observer : m_observers )
        {
            observer->on_button_pressed( m_device_state.buttons );
        }

        uchar changed = prev_state ^ current_state;
        string names = m_config.get_light_names( changed );

        LOG_DEBUG( "[device] Buttons state changed to %u (prev %u): ..... %s %s", (uint)current_state,
                   (uint)prev_state, names.c_str( ), ( current_state > prev_state ? "on" : "off" ) );
    }
}

//--------------------------------------------------------------------------------------------------

void device_t::check_lights_changes( )
{
    lights_state_t& current_state = m_device_state.lights;
    lights_state_t& prev_state = m_prev_device_state.lights;

    if ( prev_state != current_state )
    {
        for ( auto& observer : m_observers )
        {
            observer->on_light_changed( m_device_state.lights );
        }

        uchar changed = prev_state ^ current_state;
        string names = m_config.get_light_names( changed );

        LOG_DEBUG( "[device] Lights state changed to %u (prev %u): ..... %s %s", (uint)current_state,
                   (uint)prev_state, names.c_str( ), ( current_state > prev_state ? "on" : "off" ) );
    }
}

//--------------------------------------------------------------------------------------------------

void device_t::check_sensors_changes( )
{
    sensors_state_t& current_state = m_device_state.sensors;
    sensors_state_t& prev_state = m_prev_device_state.sensors;

    if ( prev_state != current_state )
    {
        for ( auto& observer : m_observers )
        {
            observer->on_sensor_triggered( m_device_state.sensors );
        }

        uchar changed = prev_state ^ current_state;
        string names = m_config.get_sensor_names( changed );

        LOG_TRACE( "[device] Sensor state changed to %u (prev %u): .... %s %s", current_state,
                   prev_state, names.c_str( ), ( current_state > prev_state ? "high" : "low" ) );
    }
}

//--------------------------------------------------------------------------------------------------

void device_t::check_double_click( )
{
    if ( m_double_click_pin != PIN_NOT_SET )
    {
        for ( auto& observer : m_observers )
        {
            observer->on_double_click( m_double_click_pin );
        }

        string name = m_config.get_light_names( 1 << m_double_click_pin );
        LOG_TRACE( "[device] Button double click %u: .... %s", m_double_click_pin, name.c_str( ) );
    }
}

//--------------------------------------------------------------------------------------------------
