#include <common/StdAfx.h>
#include <common/config.h>

#include "config_options.h"

#include <json/reader.h>


//--------------------------------------------------------------------------------------------------

config_t::config_t( )
    : m_light_pins_map( )
    , m_sensor_pins_map( )
{
}

//--------------------------------------------------------------------------------------------------

bool config_t::do_parse( )
{
    ASSERT( !m_json->isNull( ) );

    if ( !parse_lights( ) )
    {
        return false;
    }

    if ( !parse_sensors( ) )
    {
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool config_t::parse_lights( )
{
    Json::Value _lights = ( *m_json )[ "lights" ];
    if ( !_lights.isArray( ) )
    {
        LOG_ERROR( "Lights node in not an array" );
        return false;
    }

    uint count = _lights.size( );
    for ( uint idx = 0; idx < count; ++idx )
    {
        const Json::Value& light_info = _lights[ idx ];

        if ( light_info[ NAME_ATTR ].isString( ) && light_info[ PIN_ATTR ].isInt( ) )
        {
            uint pin = light_info[ PIN_ATTR ].asUInt( );
            std::string name = light_info[ NAME_ATTR ].asString( );

            m_light_pins_map[ pin ] = name;
            LOG_TRACE( "  light: %s => %u", name.c_str( ), pin );
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

bool config_t::parse_sensors( )
{
    Json::Value _sensors = ( *m_json )[ "sensors" ];
    if ( _sensors.isNull( ) || !_sensors.isArray( ) )
    {
        LOG_ERROR( "Sensors node in not an array" );
        return false;
    }

    uint count = _sensors.size( );
    for ( uint idx = 0; idx < count; ++idx )
    {
        const Json::Value& _sensor = _sensors[ idx ];

        if ( _sensor[ NAME_ATTR ].isString( ) && _sensor[ PIN_ATTR ].isInt( ) )
        {
            uint pin = _sensor[ PIN_ATTR ].asUInt( );
            std::string name = _sensor[ NAME_ATTR ].asString( );

            m_sensor_pins_map[ pin ] = name;
            LOG_TRACE( "  sensor: %s => %u", name.c_str( ), pin );
        }
    }

    return true;
}

//--------------------------------------------------------------------------------------------------

uint config_t::get_light_pin( const string& name ) const
{
    pin_config_t::const_iterator iter = m_light_pins_map.begin( );
    for ( ; iter != m_light_pins_map.end( ); ++iter )
    {
        if ( iter->second == name )
        {
            return iter->first;
        }
    }

    return -1;
}

//--------------------------------------------------------------------------------------------------

string config_t::get_light_name( lights_state_t pin ) const
{
    pin_config_t::const_iterator iter = m_light_pins_map.find( pin );
    return iter == m_light_pins_map.end( ) ? "UNKNOWN" : iter->second;
}

//--------------------------------------------------------------------------------------------------

string config_t::get_light_names( lights_state_t pins ) const
{
    string result;

    for ( size_t i = 0; i < sizeof( lights_state_t ) * 8; i++ )
    {
        bool is_on = ( pins >> i ) & 0x1;
        if ( !is_on ) continue;

        string name = get_light_name( i + 1 );
        if ( !name.empty( ) )
        {
            if ( !result.empty( ) ) result.append( ", " );
            result.append( name );
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

string config_t::get_sensor_name( sensors_state_t pin ) const
{
    pin_config_t::const_iterator iter = m_sensor_pins_map.find( pin );
    return iter == m_sensor_pins_map.end( ) ? "UNKNOWN" : iter->second;
}

//--------------------------------------------------------------------------------------------------

string config_t::get_sensor_names( sensors_state_t pins ) const
{
    string result;

    for ( size_t i = 0; i < sizeof( sensors_state_t ) * 8; i++ )
    {
        bool is_on = ( pins >> i ) & 0x1;
        if ( !is_on ) continue;

        string name = get_sensor_name( i + 1 );
        if ( !name.empty( ) )
        {
            if ( !result.empty( ) ) result.append( ", " );
            result.append( name );
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------------

const Json::Value& config_t::operator[]( const std::string& name ) const
{
    return ( *m_json )[ name ];
}

//--------------------------------------------------------------------------------------------------

