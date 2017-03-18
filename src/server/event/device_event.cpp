#include <common/StdAfx.h>

#include <event/device_event.h>


//--------------------------------------------------------------------------------------------------

device_event_t::device_event_t( DeviceEventType type, uint pin, TriggerState trigger_state,
                                uint mode, smarty::command_handler_t& command_handler,
                                const device_state_t& device_state )
    : event_t( mode )
    , m_type( type )
    , m_pin( pin )
    , m_trigger_state( trigger_state )
    , m_device_state( device_state )
    , m_prev_state( device_state )
    , m_command_handler( command_handler )
{
}

//--------------------------------------------------------------------------------------------------

/*virtual */
void device_event_t::on_event( )
{
    uint fired = 0;
    bool onOff = false;

    switch ( m_type )
    {
    case DeviceEventType::SENSOR:
        fired = ( m_device_state.sensors ^ m_prev_state.sensors ) & ( 1 << m_pin );
        onOff = fired & m_device_state.sensors;
        break;
    case DeviceEventType::BUTTON:
        fired = ( m_device_state.buttons ^ m_prev_state.buttons ) & ( 1 << m_pin );
        onOff = fired & m_device_state.buttons;
        break;
    case DeviceEventType::LIGHT:
        fired = ( m_device_state.lights ^ m_prev_state.lights ) & ( 1 << m_pin );
        onOff = fired & m_device_state.lights;
        break;
    case DeviceEventType::DOUBLE_CLICK:
        ASSERT( false && "Could not be implemented" );
        break;
    case DeviceEventType::_UNKNOWN_:
        ASSERT( false && "error" );
        break;
    }

    m_prev_state = m_device_state;

    if ( !fired )
    {
        return;
    }

    TriggerState actual = onOff ? TriggerState::HIGH : TriggerState::LOW;
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
