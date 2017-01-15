#include <common/StdAfx.h>

#include <command/command_device.h>
#include <command/command_factory.h>
#include <event/event_factory.h>
#include <event/event_parser.h>
#include <event/device_event.h>

#include <common/driver_intf.h>
#include <common/enums.h>

#include <files/config.h>


#define INVALID_PIN (uint)-1
#define INVALID_MODE (uint)-1
#define INVALID_TIMEOUT (uint)-1

#define CHECK_RETURN_MSG( _cond_, _msg_ ) \
    if ( _cond_ ) { \
        LOG_ERROR( _msg_ ); \
        return false; \
    }

#define CHECK_RETURN_FALSE( _cond_ ) \
    if ( _cond_ ) return false;


namespace
{

//--------------------------------------------------------------------------------------------------

bool is_turned_on( const std::string& value )
{
    return value == "high" || value == "on" || value == "enabled" || value == "enable";
}

//--------------------------------------------------------------------------------------------------

device_cmd_t get_device_cmd( Json::Value node )
{
    return node.type( ) == Json::stringValue
        ? is_turned_on( node.asString( ) ) ? EC_TURNON : EC_TURNOFF
        : node.asUInt( ) == 1 ? EC_TURNON : EC_TURNOFF;
}

//--------------------------------------------------------------------------------------------------

TriggerState get_sensor_state( Json::Value node )
{
    return node.type( ) == Json::stringValue
        ? is_turned_on( node.asString( ) ) ? TriggerState::HIGH : TriggerState::LOW
        : node.asUInt( ) == 1 ? TriggerState::HIGH : TriggerState::LOW;
}

//--------------------------------------------------------------------------------------------------

bool parse_pin_names( Json::Value node, std::map< std::string, uint >& pin_map )
{
    CHECK_RETURN_MSG( node.isNull( ), "Modes node does not exist" );

    pin_map.clear( );
    for ( auto mode : node )
    {
        auto _name = mode[ "name" ];
        auto _pin = mode[ "pin" ];

        CHECK_RETURN_MSG( _name.isNull( ) || _pin.isNull( ) || !_name.isString( ), 
                          "Name or pin node does not exist or is of improper type" );

        pin_map.insert( { _name.asString( ), _pin.asUInt( ) } );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

std::vector< std::string > split( const std::string& input, char delim = ' ' )
{
    std::vector< std::string > result;
    if ( input.empty( ) )
    {
        return result;
    }

    size_t start_pos = 0, end_pos = 0;
    while ( std::string::npos != ( end_pos = input.find( delim, start_pos ) ) )
    {
        result.emplace_back( input.substr( start_pos, end_pos - start_pos ) );
        start_pos = end_pos + 1;
    }

    result.emplace_back( input.substr( start_pos ) );
    return result;
}

//--------------------------------------------------------------------------------------------------

enum WhenEventType
{
    MODE_CHANGED,
    SENSOR_TRIGGERED,
    BUTTON_PRESSED,
    LIGHT_TURNED,

    _UNKNOWN_EVENT_
};

//--------------------------------------------------------------------------------------------------

enum ThenActionType
{
    ENABLE_BUTTON,
    TURN_LIGHT,
    CHANGE_MODE,

    _UNKNOWN_ACTION_
};

//--------------------------------------------------------------------------------------------------

WhenEventType get_event_type( const std::string& name )
{
    if ( name == "mode" )
    {
        return MODE_CHANGED;
    }
    else if ( name == "sensor" )
    {
        return SENSOR_TRIGGERED;
    }
    else if ( name == "button" )
    {
        return BUTTON_PRESSED;
    }
    else if ( name == "light" )
    {
        return LIGHT_TURNED;
    }

    return _UNKNOWN_EVENT_;
}

//--------------------------------------------------------------------------------------------------

ThenActionType get_action_type( const std::string& name )
{
    if ( name == "button" )
    {
        return ENABLE_BUTTON;
    }
    else if ( name == "light" )
    {
        return TURN_LIGHT;
    }
    else if ( name == "mode" )
    {
        return CHANGE_MODE;
    }

    return _UNKNOWN_ACTION_;
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

event_parser_t::~event_parser_t( )
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
    CHECK_RETURN_MSG( node.isNull( ), "Mode node is null" );

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

std::string event_parser_t::parse_mode_text( const std::string& condition, uint& mode )
{
    if ( condition.empty( ) )
    {
        LOG_ERROR( "When condition is empty" );
        return condition;
    }

    std::string result;

    size_t pos = condition.find( " in " );
    if ( pos != std::string::npos )
    {
        result = condition.substr( 0, pos );

        auto words = split( condition.substr( pos + 4 ) );
        if ( words.size( ) < 2 )
        {
            LOG_ERROR( "Incorrect mode name in when condition \'%s\'", condition.c_str( ) );
        }
        else
        {
            mode = get_mode_id( words[ 0 ] );
        }
    }
    else
    {
        mode = 0;
        result = condition;
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

std::string event_parser_t::parse_timeout_text( const std::string& action, uint& timeout )
{
    if ( action.empty( ) )
    {
        LOG_ERROR( "Then action is empty" );
        return action;
    }

    std::string result;

    size_t pos = action.find( " with delay " );
    static size_t len = strlen( " with delay " );

    if ( pos != std::string::npos )
    {
        result = action.substr( 0, pos );

        auto words = split( action.substr( pos + len ) );
        if ( words.size( ) < 2 )
        {
            LOG_ERROR( "Incorrect delay in then action \'%s\'", action.c_str( ) );
        }
        else
        {
            timeout = atol( words[ 0 ].c_str( ) );
            // TODO: support minutes/hours -- words[ 2 ]
        }
    }
    else
    {
        timeout = 0;
        result = action;
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
    if ( !_actions.isNull( ) )
    {
        return parse_actions_json( _actions, actions );
    }

    auto _then = node[ "then" ];
    if ( !_then.isNull( ) )
    {
        return parse_actions_text( _then, actions );
    }

    LOG_ERROR( "Event actions were not set" );
    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_actions_json( Json::Value _actions, actions_t& actions )
{
    CHECK_RETURN_MSG( !_actions.isArray( ), "Actions node is not of array type" );

    for ( auto action : _actions )
    {
        if ( !parse_action_json( action, actions ) )
        {
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_action_json( Json::Value action, actions_t& actions)
{
    auto _target = action[ "target" ];
    CHECK_RETURN_MSG( _target.isNull( ), "\'target\' not not set in action node" );
    auto _turn_on = action[ "turn on" ];
    CHECK_RETURN_MSG( _turn_on.isNull( ), " \'turn on\' node not set in button action" );

    auto _delay = action[ "delay" ];
    uint delay = _delay.isNull( ) ? 0 : _delay.asUInt( );

    uint pin = INVALID_PIN;
    switch ( get_action_type( _target.asString( ) ) )
    {
    case TURN_LIGHT:
        pin = parse_light_pin( action );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid light name in light action" );
        add_device_command( actions, get_device_cmd( _turn_on ), 1 << pin, delay );
        break;
    case ENABLE_BUTTON:
        pin = parse_light_pin( action );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid button name in button action" );
        add_device_command( actions, _turn_on.asBool( ) ? EC_TURNONBUTTON : EC_TURNOFFBUTTON,
                            1 << pin, delay );
        break;
    case CHANGE_MODE:
        ASSERT( false && "Change Mode action Not implemented yet ");
        break;
    default:
        ASSERT( false && "TargetType case is not implemented" );
    }

    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_actions_text( Json::Value node, actions_t& actions )
{
    if ( node.isString( ) )
    {
        return parse_action_text( node, actions );
    }

    if ( node.isArray( ) )
    {
        return parse_actions_array_text( node, actions );
    }

    LOG_ERROR( "Incorrect type of \'then\' node" );
    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_action_text( Json::Value node, actions_t& actions )
{
    CHECK_RETURN_MSG( !node.isString( ), "text action is of incorrect type" );

    uint timeout = INVALID_TIMEOUT;
    auto action = parse_timeout_text( node.asString( ), timeout );
    CHECK_RETURN_MSG( timeout == INVALID_TIMEOUT,
                      ( "Incorrect delay string \'%s\'" + node.asString( ) ).c_str( ) );

    auto words = split( action );
    CHECK_RETURN_MSG( words.size( ) < 3,
        ( "too few words in then action: " + node.asString( ) ).c_str( ) );

    size_t last = words.size( ) - 1;
    const auto& type = words[ last ];
    const auto& name = words[ last - 1 ];
    bool turned_on = is_turned_on( words[ last - 2 ] );

    uint pin = INVALID_PIN;
    switch ( get_action_type( type ) )
    {
    case ENABLE_BUTTON:
        pin = parse_light_pin( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid button name in then action" );
        add_device_command( actions, turned_on ? EC_TURNONBUTTON : EC_TURNOFFBUTTON,
                            static_cast< device_param_t >( 1 << pin ) , timeout );
        break;
    case TURN_LIGHT:
        pin = parse_light_pin( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid light name in then action" );
        add_device_command( actions, turned_on ? EC_TURNON : EC_TURNOFF,
                            static_cast< device_param_t >( 1 << pin ) , timeout );
        break;
    case CHANGE_MODE:
        pin = get_mode_id( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid mode name in then action" );
        ASSERT( false && "Change Mode action Not implemented yet ");
        break;
    case _UNKNOWN_ACTION_:
        LOG_ERROR( "Unknown then action type: \'%s\'", type.c_str( ) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_actions_array_text( Json::Value _actions, actions_t& actions )
{
    for ( auto action : _actions )
    {
        if ( !parse_actions_text( action, actions ) )
        {
            return false;
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_condition( Json::Value node, std::shared_ptr< smarty::event_t >& event )
{
    auto _condition = node[ "condition" ];
    if ( !_condition.isNull( ) )
    {
        return parse_condition_json( _condition, event );
    }

    auto _when = node[ "when" ];
    if ( !_when.isNull( ) )
    {
        return parse_condition_text( _when, event );
    }

    LOG_ERROR( "Event condition was not set" );
    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_condition_json( Json::Value node,
                                           std::shared_ptr< smarty::event_t >& event )
{
    uint event_mode;
    if ( !parse_mode( node, event_mode ) )
    {
        return false;
    }

    auto _device = node[ "device" ];
    if ( !_device.isNull( ) )
    {
        return parse_device_condition( _device, event, event_mode );
    }

    auto _mode = node[ "mode_change" ];
    if ( !_mode.isNull( ) )
    {
        return parse_mode_condition( _mode, event );
    }

    LOG_ERROR( "Device or mode node not set in condition node" );
    return false;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_condition_text( Json::Value node,
                                           std::shared_ptr< smarty::event_t >& event )
{
    CHECK_RETURN_MSG( !node.isString( ), "when node is not of string type" );

    uint event_mode = INVALID_MODE;
    std::string condition = parse_mode_text( node.asString( ), event_mode );
    CHECK_RETURN_MSG( event_mode == INVALID_MODE,
                      ( "Invalid mode in when condition" + node.asString( ) ).c_str( ) );

    auto words = split( condition );
    CHECK_RETURN_MSG( words.size( ) < 3,
        ( "too few words in when condition: " + node.asString( ) ).c_str( ) );

    const auto& type = words[ 1 ];
    const auto& name = words[ 0 ];
    // take last word
    size_t last = words.size( ) - 1;

    TriggerState turned_on = is_turned_on( words[ last ] ) ? TriggerState::HIGH : TriggerState::LOW;

    uint pin = INVALID_PIN;
    switch ( get_event_type( type ) )
    {
    case MODE_CHANGED:
        pin = get_mode_id( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid mode name in when node" );
        event = m_event_factory.create_mode_event( pin, turned_on == TriggerState::HIGH );
        break;
    case SENSOR_TRIGGERED:
        pin = parse_sensor_pin( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid sensor name in when node" );
        event = m_event_factory.create_device_event(
                    DeviceEventType::SENSOR, pin, turned_on, event_mode );
        break;
    case BUTTON_PRESSED:
        pin = parse_light_pin( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid button name in when node" );
        event = m_event_factory.create_device_event(
                    DeviceEventType::BUTTON, pin, turned_on, event_mode );
        break;
    case LIGHT_TURNED:
        pin = parse_light_pin( name );
        CHECK_RETURN_MSG( pin == INVALID_PIN, "Invalid light name in when node" );
        event = m_event_factory.create_device_event(
                    DeviceEventType::BUTTON, pin, turned_on, event_mode );
        break;
    case _UNKNOWN_EVENT_:
        LOG_ERROR( "Unknown when condition type: \'%s\'", type.c_str( ) );
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_device_condition( Json::Value node,
                                             std::shared_ptr< smarty::event_t >& event, uint mode )
{
    auto _type = node[ "type" ];
    CHECK_RETURN_MSG( _type.isNull( ), "Device/type node not set in event node" );

    const std::string type = _type.asString( );
    DeviceEventType event_type = device_event_type_by_name( type );

    CHECK_RETURN_MSG( event_type == DeviceEventType::_UNKNOWN_,
                    ( "Incorrect device/type node type: " + type ).c_str( ) );

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
    }

    CHECK_RETURN_FALSE( pin == INVALID_PIN );

    auto _state = node[ "state" ];
    CHECK_RETURN_MSG( _state.isNull( ), "State node not set in sensor node" );

    TriggerState trigger_state = get_sensor_state( _state );
    event = m_event_factory.create_device_event( event_type, pin, trigger_state, mode );

    return true;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_mode_condition( Json::Value node,
                                           std::shared_ptr< smarty::event_t >& event )
{
    auto _name = node[ "name" ];
    CHECK_RETURN_MSG( _name.isNull( ), "Mode name not set in event node" );

    uint mode_id = get_mode_id( _name.asString( ) );
    CHECK_RETURN_MSG( mode_id == INVALID_MODE,
                      ( "Event mode " + _name.asString( ) + " name does not exist" ).c_str( ) );

    auto _changed_to = node[ "to" ];
    CHECK_RETURN_MSG( _changed_to.isNull(), "Mode event \'to\' attribute not set in event node" );

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
        pin = parse_sensor_pin( _name.asString( ) );
    }
    else
    {
        LOG_ERROR( "Pin and name nodes not set in sensor node" );
        return INVALID_PIN;
    }

    return pin;
}

//--------------------------------------------------------------------------------------------------

uint event_parser_t::parse_sensor_pin( const std::string& name )
{
    auto it = m_sensors.find( name );
    if ( it == m_sensors.end( ) )
    {
        LOG_ERROR( "Sensor with name \'%s\' does not exist", name.c_str( ) );
        return INVALID_PIN;
    }

    return it->second - 1;
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
        pin = parse_light_pin( _name.asString( ) );
    }
    else
    {
        LOG_ERROR( "Pin and name nodes not set in lights node" );
        return INVALID_PIN;
    }

    return pin;
}

//--------------------------------------------------------------------------------------------------

uint event_parser_t::parse_light_pin( const std::string& name )
{
    auto it = m_lights.find( name );
    if ( it == m_lights.end( ) )
    {
        LOG_ERROR( "Light with name \'%s\' does not exist", name.c_str( ) );
        return INVALID_PIN;
    }

    return it->second - 1;
}

//--------------------------------------------------------------------------------------------------

bool event_parser_t::parse_modes( Json::Value node )
{
    CHECK_RETURN_MSG( node.isNull( ), "Modes node does not exist" );

    m_modes.clear( );
    m_modes.emplace_back( "all" );

    for ( auto mode : node )
    {
        auto _name = mode[ "name" ];
        CHECK_RETURN_MSG( _name.isNull( ) || !_name.isString( ),
                          "Mode name does not exist or is not string" );

        m_modes.emplace_back( _name.asString() );
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

void event_parser_t::add_device_command( actions_t& actions,
                                         device_cmd_t cmd, device_param_t param, uint timeout )
{
    actions.push_back( m_command_factory.create_device_command( { cmd, param }, timeout ) );
}

//--------------------------------------------------------------------------------------------------
