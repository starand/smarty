#include <common/StdAfx.h>

#include <command/command_device.h>
#include <command/command_factory.h>
#include <event/event_factory.h>
#include <event/event_parser.h>
#include <event/sensor_event.h>

#include <common/driver_intf.h>
#include <common/enums.h>

#include <files/config.h>


#define INVALID_PIN (uint)-1
#define INVALID_MODE (uint)-1

namespace
{

//--------------------------------------------------------------------------------------------------

device_cmd_t get_device_cmd( Json::Value node )
{
    return node.type( ) == Json::stringValue
        ? node.asString( ) == "high" || node.asString( ) == "on" ? EC_TURNON : EC_TURNOFF
        : node.asUInt( ) == 1 ? EC_TURNON : EC_TURNOFF;
}

//--------------------------------------------------------------------------------------------------

TriggerState get_sensor_state( Json::Value node )
{
    return node.type( ) == Json::stringValue
        ? node.asString( ) == "high" || node.asString( ) == "on"
            ? TriggerState::HIGH : TriggerState::LOW
        : node.asUInt( ) == 1 ? TriggerState::HIGH : TriggerState::LOW;
}

//--------------------------------------------------------------------------------------------------

bool parse_pin_names( Json::Value node, std::map< std::string, uint >& pin_map )
{
    if ( node.isNull( ) )
    {
        LOG_ERROR("Modes node does not exist");
        return false;
    }

    pin_map.clear( );
    for ( auto mode : node )
    {
        auto _name = mode[ "name" ];
        auto _pin = mode[ "pin" ];

        if ( _name.isNull( ) || _pin.isNull( ) || !_name.isString( ) )
        {
            LOG_ERROR( "Name or pin node does not exist or is of improper type" );
            return false;
        }

        pin_map.insert( { _name.asString( ), _pin.asUInt( ) } );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

}

//--------------------------------------------------------------------------------------------------

std::vector< std::string > event_parser_t::m_modes;

//--------------------------------------------------------------------------------------------------

event_parser_t::event_parser_t( smarty::event_factory_t& event_factory,
                                smarty::command_factory_t& command_factory )
    : m_event_factory( event_factory )
    , m_command_factory( command_factory )
    , m_sensors( )
    , m_lights( )
{
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse( const config_t& config )
{
    if ( !parse_modes( config[ "modes" ] ) )
    {
        return false;
    }

    if ( !parse_pin_names( config[ "sensors" ], m_sensors ) ||
         !parse_pin_names( config[ "lights" ], m_lights ) )
    {
        return false;
    }

    auto events_node = config[ "events" ];
    if ( events_node.isNull( ) )
    {
        return true;
    }

    uint size = events_node.size( );
    for ( uint idx = 0; idx < size; ++idx )
    {
        if ( !parse_event( events_node[ idx ] ) )
        {
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_event( Json::Value node )
{
    std::shared_ptr< smarty::event_t > event;
    if ( !parse_condition( node, event ) )
    {
        return false;
    }

    std::vector< std::shared_ptr< smarty::command_t > > actions;
    if ( !parse_actions( node, actions ) )
    {
        return false;
    }

    event->set_actions( actions );
    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_mode( Json::Value node, uint& mode )
{
    if ( node.isNull( ) )
    {
        LOG_ERROR( "Event condition not set" );
        return false;
    }

    mode = 0;
    auto _mode = node[ "mode" ];

    if ( !_mode.isNull( ) )
    {
        uint res = get_mode_id( _mode.asString( ) );
        if ( res == INVALID_MODE )
        {
            LOG_ERROR("Event mode \'%s\' does not exist", _mode.asString( ).c_str( ) );
            return false;
        }

        mode = res;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

uint event_parser_t::get_mode_id( const std::string& mode_name )
{
    uint result = INVALID_MODE;

    const uint size = (uint)m_modes.size( );
    for ( uint idx = 0; idx < size; ++idx )
    {
        if ( m_modes[ idx ] == mode_name )
        {
            result = idx;
            break;
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

const std::vector< std::string > event_parser_t::get_modes( )
{
    return m_modes;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_actions( Json::Value node, actions_t& actions )
{
    auto _actions = node[ "actions" ];
    if ( _actions.isNull( ) || !_actions.size( ) )
    {
        LOG_ERROR( "Event actions not set" );
        return false;
    }

    actions.reserve( _actions.size( ) );

    for ( auto action : _actions )
    {
        if ( !parse_action( action, actions ) )
        {
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_action( Json::Value action, actions_t& actions)
{
    auto _target = action[ "target" ];
    if ( _target.isNull( ) )
    {
        LOG_ERROR( "target not not set in action node" );
        return false;
    }

    std::string target_str = _target.asString( );
    TargetType target = target_type_by_name( target_str.c_str( ) );
    if ( target == TargetType::_UNKNOWN_ )
    {
        LOG_ERROR( "Incorrect target type: \'%s\'", target_str.c_str( ) );
        return false;
    }

    bool error = false;
    switch ( target )
    {
    case TargetType::LIGHT:
        error = !parse_light_action( action, actions );
        break;
    case TargetType::BUTTON:
        error = !parse_button_action( action, actions );
        break;
    case TargetType::SENSOR:
        ASSERT( false && "do not see for now any reasonable actions related with sensors" );
    default:
        ASSERT( false && "TargetType switch case is not implemented" );
    }

    return !error;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_light_action( Json::Value action, actions_t& actions )
{
    uint light_pin = parse_light_pin( action );
    if ( light_pin == INVALID_PIN )
    {
        return false;
    }

    auto _set_state = action[ "set_state" ];
    if ( _set_state.isNull( ) )
    {
        LOG_ERROR( "set_state node not set in light action" );
        return false;
    }

    device_cmd_t cmd = get_device_cmd( _set_state );
    device_param_t param = 1 << light_pin;

    auto _delay = action[ "delay" ];
    uint timeout = _delay.isNull( ) ? 0 : _delay.asUInt( );

    auto command = m_command_factory.create_device_command( { cmd, param }, timeout );
    actions.push_back( command );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_button_action( Json::Value action, actions_t& actions )
{
    uint button_pin = parse_light_pin( action );
    if ( button_pin == INVALID_PIN )
    {
        return false;
    }

    auto _enable = action[ "enable" ];
    if ( _enable.isNull( ) )
    {
        LOG_ERROR( "enable node not set in button action" );
        return false;
    }

    device_param_t param = 1 << button_pin;
    bool enable = _enable.asBool( );

    device_cmd_t cmd = enable ? EC_TURNONBUTTON : EC_TURNOFFBUTTON;
    auto command = m_command_factory.create_device_command( { cmd, param }, 0 );
    actions.push_back( command );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_condition( Json::Value node, std::shared_ptr< smarty::event_t >& event )
{
    auto _condition = node[ "condition" ];
    uint event_mode;
    if ( !parse_mode( _condition, event_mode ) )
    {
        return false;
    }

    auto _device = _condition[ "device" ];
    if ( !_device.isNull( ) )
    {
        return parse_device_condition( _device, event, event_mode );
    }

    auto _mode = _condition[ "mode_change" ];
    if ( !_mode.isNull( ) )
    {
        return parse_mode_condition( _mode, event );
    }

    LOG_ERROR( "Device or mode node not set in condition node" );
    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_device_condition( Json::Value node,
                                             std::shared_ptr< smarty::event_t >& event, uint mode )
{
    auto _type = node[ "type" ];
    if ( _type.isNull( ) )
    {
        LOG_ERROR( "Device/type node not set in event node" );
        return false;
    }

    const std::string type = _type.asString( );
    DeviceEventType event_type = device_event_type_by_name( type );
    if ( event_type == DeviceEventType::_UNKNOWN_ )
    {
        LOG_ERROR( "Incorrect device/type node type \'%s\'", type.c_str( ) );
        return false;
    }

    uint pin = INVALID_PIN;
    switch ( event_type )
    {
    case DeviceEventType::SENSOR:
        pin = parse_sensor_pin( node );
        break;
    case DeviceEventType::BUTTON:
    case DeviceEventType::LIGHT:
        pin = parse_light_pin( node );
        break;
    case DeviceEventType::_UNKNOWN_:
        break;
    }

    if ( pin == INVALID_PIN )
    {
        return false;
    }

    auto _state = node[ "state" ];
    if ( _state.isNull( ) )
    {
        LOG_ERROR( "State node not set in sensor node" );
        return false;
    }

    TriggerState trigger_state = get_sensor_state( _state );

    switch ( event_type )
    {
    case DeviceEventType::SENSOR:
        event = m_event_factory.create_sensor_event( pin, trigger_state, mode );
        break;
    case DeviceEventType::BUTTON:
        event = m_event_factory.create_button_event( pin, trigger_state, mode );
        break;
    case DeviceEventType::LIGHT:
        event = m_event_factory.create_light_event( pin, trigger_state, mode );
        break;
    default:
        ASSERT( false && "Incorrect device event type" );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_mode_condition( Json::Value node,
                                           std::shared_ptr< smarty::event_t >& event )
{
    auto _name = node[ "name" ];
    if ( _name.isNull( ) )
    {
        LOG_ERROR( "Mode name not set in event node" );
        return false;
    }

    uint mode_id = get_mode_id( _name.asString( ) );
    if ( mode_id == INVALID_MODE )
    {
        LOG_ERROR("Event mode \'%s\' name does not exist", _name.asString( ).c_str( ) );
        return false;
    }

    auto _changed_to = node[ "to" ];
    if ( _changed_to.isNull() )
    {
        LOG_ERROR( "Mode event \'to\' attribute not set in event node" );
        return false;
    }

    bool enabled = _changed_to.asString() == "on";
    event = m_event_factory.create_mode_event( mode_id, enabled );

    return true;
}

//--------------------------------------------------------------------------------------------------

uint event_parser_t::parse_sensor_pin( Json::Value node )
{
    uint pin = INVALID_PIN;

    auto _pin = node[ "pin" ];
    auto _name = node[ "name" ];
    if ( !_pin.isNull( ) )
    {
        pin = _pin.asUInt( ) - 1;
    }
    else if ( !_name.isNull( ) && _name.isString( ) )
    {
        const std::string name = _name.asString( );
        auto it = m_sensors.find( name );
        if ( it == m_sensors.end( ) )
        {
            LOG_ERROR( "Sensor with name \'%s\' does not exist", name.c_str( ) );
            return INVALID_PIN;
        }

        pin = it->second - 1;
    }
    else
    {
        LOG_ERROR( "Pin and name nodes not set in sensor node" );
        return INVALID_PIN;
    }

    return pin;
}

//--------------------------------------------------------------------------------------------------

uint event_parser_t::parse_light_pin( Json::Value node )
{
    uint pin = INVALID_PIN;

    auto _pin = node[ "pin" ];
    auto _name = node[ "name" ];
    if ( !_pin.isNull( ) )
    {
        pin = _pin.asUInt( ) - 1;
    }
    else if ( !_name.isNull( ) && _name.isString( ) )
    {
        const std::string name = _name.asString( );
        auto it = m_lights.find( name );
        if ( it == m_lights.end( ) )
        {
            LOG_ERROR( "Light with name \'%s\' does not exist", name.c_str( ) );
            return INVALID_PIN;
        }

        pin = it->second - 1;
    }
    else
    {
        LOG_ERROR( "Pin and name nodes not set in lights node" );
        return INVALID_PIN;
    }

    return pin;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_modes( Json::Value node )
{
    if ( node.isNull( ) )
    {
        LOG_ERROR("Modes node does not exist");
        return false;
    }

    m_modes.clear( );
    m_modes.emplace_back( "all" );

    for ( auto mode : node )
    {
        auto _name = mode[ "name" ];
        if ( _name.isNull( ) || !_name.isString( ) )
        {
            LOG_ERROR( "Mode name does not exist or is not string" );
            return false;
        }

        m_modes.emplace_back( _name.asString() );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
