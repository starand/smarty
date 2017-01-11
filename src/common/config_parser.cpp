#include <common/StdAfx.h>
#include "config_parser.h"

#include <json/reader.h>


const char LIGHT[] = "lights";
const char NAME[] = "name";
const char PIN[] = "pin";
const char AUTOMATIC[] = "automatic";


//////////////////////////////////////////////////
// config_parser_t implementation

config_parser_t::config_parser_t( const string& config )
    : m_lights( )
    , m_parsed( false )
    , m_config( config )
{
}

config_parser_t::~config_parser_t( )
{

}


bool config_parser_t::parse( )
{
    Json::Reader reader;
    Json::Value config_value;

    if ( reader.parse( m_config, config_value ) && !config_value.isNull( ) )
    {
        m_parsed = true;
        m_lights = config_value[ LIGHT ];
    }

    return m_parsed;
}

bool config_parser_t::is_parsed( ) const
{
    return m_parsed;
}


sizeint config_parser_t::get_lights_count( ) const
{
    if ( !m_parsed || !m_lights.isArray( ) )
    {
        return 0;
    }

    return (sizeint)m_lights.size( );
}

//--------------------------------------------------------------------------------------------------

const Json::Value& config_parser_t::operator[]( const std::string& name ) const
{
    return m_lights[ name ];
}

//--------------------------------------------------------------------------------------------------

bool config_parser_t::get_light_info( sizeint index, light_info_t& info ) const
{
    if ( !m_parsed )
    {
        return false;
    }

    if ( index > get_lights_count( ) )
    {
        return false;
    }

    const Json::Value& light_info = m_lights[ index ];

    if ( light_info[ NAME ].isString( ) )
    {
        info.name = light_info[ NAME ].asString( );
    }

    if ( light_info[ PIN ].isInt( ) )
    {
        info.pin = (sizeint)light_info[ PIN ].asUInt( );
    }

    info.automatic_set = !light_info[ AUTOMATIC ].isNull();

    return true;
}

//--------------------------------------------------------------------------------------------------
