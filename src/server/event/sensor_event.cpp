#include <common/StdAfx.h>

#include <command/command_handler.h>
#include <device/light_object.h>
#include <event/sensor_event.h>


//--------------------------------------------------------------------------------------------------

sensor_event_t::sensor_event_t( uint pin, TriggerState trigger_state, uint mode,
                                smarty::command_handler_t& command_handler,
                                const device_state_t& device_state )
    : m_sensor_pin( pin )
    , m_trigger_state( trigger_state )
    , m_mode( mode )
    , m_device_state( device_state )
    , m_prev_state( device_state )
    , m_command_handler( command_handler )
{
}

//--------------------------------------------------------------------------------------------------

sensor_event_t::~sensor_event_t( )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void sensor_event_t::on_event( )
{
    uint fired = ( m_device_state.sensors ^ m_prev_state.sensors ) & ( 1 << m_sensor_pin );
    m_prev_state = m_device_state;

    if ( !fired )
    {
        return;
    }

    TriggerState actual = fired & m_device_state.sensors ? TriggerState::HIGH : TriggerState::LOW;
    if ( actual != m_trigger_state )
    {
        return;
    }

    for ( auto& action : m_actions )
    {
        m_command_handler.add_command( action );
    }
}

//--------------------------------------------------------------------------------------------------

/*virtual */
uint sensor_event_t::get_mode( )
{
    return m_mode;
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void sensor_event_t::set_actions( std::vector< std::shared_ptr< smarty::command_t > >& acts )
{
    m_actions.swap( acts );
}

//--------------------------------------------------------------------------------------------------
